// Copyright [2023] <Kdimo>
#include <assert.h>
#include <unistd.h>
#include <sys/event.h>
#include <sys/socket.h>

#include "../../include/util/util.h"
#include "../../include/event/event.hpp"

namespace Event {
  int IEventHandle::register_event(fd_t fd, EventType type) {
    return CEventProxy::instance()->register_event(fd, this, type);
  }

  int IEventHandle::register_event(Socket::ISocket* socket, EventType type) {
    return CEventProxy::instance()->register_event(socket, this, type);
  }

  int IEventHandle::shutdown_event(int fd) {
    return CEventProxy::instance()->shutdown_event(fd);
  }

  int IEventHandle::shutdown_event(Socket::ISocket* socket) {
    return CEventProxy::instance()->shutdown_event(socket);
  }

  CEventProxy::CEventProxy(size_t event_max) {
    m_event.reset(new CEvent(event_max));
  }

  CEventProxy::~CEventProxy() {
    if (m_event) {
      m_event.release();
    }
  }

  CEventProxy* CEventProxy::instance() {
    static CEventProxy* event_proxy(nullptr);

    if (event_proxy == nullptr) {
      event_proxy = new CEventProxy(NEVENT_MAX);
    }

    return event_proxy;
  }

  int CEventProxy::register_event(fd_t fd, IEventHandle* handle,
      EventType type) {
    return m_event->register_event(fd, handle, type);
  }

  int CEventProxy::register_event(Socket::ISocket* socket,
      IEventHandle* handle, EventType type) {
    return register_event(socket->fd(), handle, type);
  }

  int CEventProxy::shutdown_event(fd_t fd) {
    return m_event->shutdown_event(fd);
  }

  int CEventProxy::shutdown_event(Socket::ISocket* socket) {
    return shutdown_event(socket->fd());
  }

  CNetObserver::CNetObserver(INetObserver& obj, EventType type):
    m_reg_event(type), m_obj(obj), m_refcount(1) {}
  CNetObserver::~CNetObserver() {}

  void CNetObserver::addref() {
    std::lock_guard<std::mutex> guard(m_refcount_mutex);
    m_refcount++;
  }

  void CNetObserver::subref() {
    std::lock_guard<std::mutex> guard(m_refcount_mutex);
    m_refcount--;
  }

  bool CNetObserver::subref_test() {
    std::lock_guard<std::mutex> guard(m_refcount_mutex);
    m_refcount--;

    return (m_refcount == 0);
  }

  EventType CNetObserver::get_reg_event() {
    return m_reg_event;
  }

  const INetObserver* CNetObserver::get_handle() {
    return &m_obj;
  }

  void CNetObserver::handle_in(fd_t fd) {
    m_obj.handle_in(fd);
  }

  void CNetObserver::handle_out(fd_t fd) {
    m_obj.handle_out(fd);
  }

  void CNetObserver::handle_close(fd_t fd) {
    m_obj.handle_close(fd);
  }

  void CNetObserver::handle_error(fd_t fd) {
    m_obj.handle_error(fd);
  }

  CEvent::CEvent(size_t event_size) {
    m_detection_thread = 0;
    bzero(reinterpret_cast<void*>(&m_event_buffer), sizeof(m_event_buffer));

    m_kqueue_fd = kqueue();
    assert(m_kqueue_fd >= 0);

    m_threadpool.reset(Thread::CThreadpoolProxy::instance());

    pthread_t tid = 0;
    if (pthread_create(&tid, nullptr, event_wait_thread,
          reinterpret_cast<void*>(this)) == 0) {
      m_detection_thread = tid;
    }
  }

  CEvent::~CEvent() {
    if (m_detection_thread != 0 && pthread_cancel(m_detection_thread) == 0) {
      (void) pthread_join(m_detection_thread, nullptr);
    }

    if (!INVALID_FD(m_kqueue_fd)) {
      close(m_kqueue_fd);
    }
  }

  int CEvent::register_event(fd_t fd, IEventHandle* handle, EventType type) {
    if (INVALID_FD(fd) || INVALID_FD(m_kqueue_fd) || INVALID_POINTER(handle)) {
      seterrno(EINVAL);
      return -1;
    }

    ExistRet ret = is_exist(fd, type, handle);
    if (ret == EXISTED) {
      return 0;
    }

    add_observer(fd, type, handle);
    if (ret == HANDLE_MODIFY) {
      return 0;
    }

    struct kevent new_event;
    EV_SET(&new_event, fd, type, EV_ADD, 0, 0, 0);

    if (kevent(m_kqueue_fd, &new_event, 1, nullptr, 0, nullptr) == -1) {
      errsys("Event center: failed to register new event opt=%d, fd=%d",
         EV_ADD, fd);
      remove_observer(fd, type, true);

      return -1;
    }

    return 0;
  }

  int CEvent::shutdown_event(fd_t fd) {
    trace("Event center: sock[%#X] shutdown event\n", fd);
    return ::shutdown(fd, SHUT_WR);
  }

  CEvent::ExistRet CEvent::is_exist(fd_t fd, EventType type, IEventHandle* handle) {  // cpplint: NOLINT
    std::lock_guard<std::mutex> guard(m_reg_event_mutex);

    event_identify_t event_ident = {fd, type};
    auto it = m_reg_event.find(event_ident);

    if (it == m_reg_event.end()) {
      return NOT_EXIST;
    }

    CNetObserver& cur_ob = *(it->second);
    const INetObserver* cur_handle = cur_ob.get_handle();

    if (handle != cur_handle) {
      return HANDLE_MODIFY;
    }

    return EXISTED;
  }

  int CEvent::add_observer(fd_t fd, EventType type, IEventHandle* handle) {
    std::shared_ptr<CNetObserver> new_observer(new CNetObserver(*handle, type));
    assert(new_observer != nullptr);

    std::lock_guard<std::mutex> guard(m_reg_event_mutex);
    m_reg_event[{fd, type}] = new_observer;

    return 0;
  }

  int CEvent::remove_observer(fd_t fd, EventType type, bool release) {
    std::lock_guard<std::mutex> guard(m_reg_event_mutex);
    auto it = m_reg_event.find({fd, type});

    if (it != m_reg_event.end()) {
      if (release) {
        it->second.reset();
      }
      m_reg_event.erase(it);
    } else {
      return -1;
    }

    return 0;
  }

  std::shared_ptr<CNetObserver> CEvent::get_observer(fd_t fd, EventType type) {
    std::lock_guard<std::mutex> guard(m_reg_event_mutex);
    auto it = m_reg_event.find({fd, type});

    if (it != m_reg_event.end()) {
      it->second->addref();

      return it->second;
    }

    return nullptr;
  }

  int CEvent::push_event_trigger(const struct kevent& event) {
    std::lock_guard<std::mutex> guard(m_events_mutex);
    event_identify_t ident = {event.ident, (EventType)event.filter};

    if (m_events_map[ident] == false) {
      m_events.push(event);
      m_events_map[ident] = true;

      return 0;
    } else {
      return -1;
    }
  }

  int CEvent::pop_event_trigger(struct kevent& event) {
    std::lock_guard<std::mutex> guard(m_events_mutex);

    if (m_events.size()) {
      event = m_events.front();
      m_events.pop();
      m_events_map[{event.ident, (EventType)event.filter}] = false;

      return 0;
    } else {
      return -1;
    }
  }

  size_t CEvent::task_size() {
    std::lock_guard<std::mutex> guard(m_events_mutex);
    return m_events.size();
  }

  int CEvent::erase_task() {
    std::lock_guard<std::mutex> guard(m_events_mutex);
    std::queue<struct kevent> empty_queue;
    std::swap(m_events, empty_queue);

    return 0;
  }

  int CEvent::unregister_event(fd_t fd, EventType type) {
    struct kevent event;
    EV_SET(&event, fd, type, EV_DELETE, 0, 0, 0);

    if (kevent(m_kqueue_fd, &event, 1, nullptr, 0, nullptr) == -1) {
      errsys("delete event failed fd=%d type=%d", fd, type);
      return -1;
    }

    return remove_observer(fd, type);
  }

  void CEvent::handle_task() {
    struct kevent event;
    if (pop_event_trigger(event) != 0) {
      return;
    }

    std::shared_ptr<CNetObserver> observer =
      get_observer(event.ident, (EventType)event.filter);

    if (observer == nullptr) {
      return;
    }

    if (event.flags & EV_ERROR) {
      observer->handle_error(event.ident);
    } else if (event.flags & EV_EOF) {
      observer->subref();
    } else if (event.filter & EVFILT_READ) {
      observer->handle_in(event.ident);
    } else if (event.filter & EVFILT_WRITE) {
      observer->handle_out(event.ident);
    }

    if (observer->subref_test()) {
      unregister_event(event.ident, (EventType)event.filter);
      observer->handle_close(event.ident);
      observer.reset();
    }
  }

  void* CEvent::event_wait_thread(void* arg) {
    CEvent& event = *reinterpret_cast<CEvent*>(arg);
    if (INVALID_FD(event.m_kqueue_fd)) {
      seterrno(EINVAL);
      pthread_exit(nullptr);
    }

    for (;;) {
      fd_t n_event = kevent(event.m_kqueue_fd, nullptr, 0,
          event.m_event_buffer, EVENT_BUFF_LEN, nullptr);

      if (n_event < 0 && errno != EINTR) {
        errsys("event wait error\n");
        break;
      }

      for (fd_t i = 0; i < n_event; i++) {
        if (event.push_event_trigger(event.m_event_buffer[i]) == 0) {
          event.m_threadpool->push_task(&event);
        }
      }
    }

    pthread_exit(nullptr);
  }
}  // namespace Event
