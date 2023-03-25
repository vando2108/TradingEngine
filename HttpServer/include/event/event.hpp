// Copyright [2023] <Kdimo>
#ifndef __EVENT_H__  // cplint: NOLINT
#define __EVENT_H__

#include <memory>
#include <queue>
#include <utility>
#include <unordered_map>

#include "../socket/socket.h"
#include "../thread/thread.h"
#include "../../include/util/util.h"

// Internal include
#include "./enum.hpp"

namespace Event {
class INetObserver {
  friend class CNetObserver;

 public:
  virtual ~INetObserver() {}

 protected:
  virtual void handle_in(fd_t) = 0;
  virtual void handle_out(fd_t) = 0;
  virtual void handle_close(fd_t) = 0;
  virtual void handle_error(fd_t) = 0;
};

class CNetObserver: public INetObserver {
  friend class CEvent;

 public:
  CNetObserver(INetObserver&, EventType);
  ~CNetObserver();

  inline void addref();
  inline void subref();
  inline bool subref_test();
  inline EventType get_reg_event();
  inline const INetObserver* get_handle();

 protected:
  void handle_in(fd_t);
  void handle_out(fd_t);
  void handle_close(fd_t);
  void handle_error(fd_t);

 private:
  EventType m_reg_event;
  INetObserver &m_obj;

  int32_t m_refcount;
  std::mutex m_refcount_mutex;
};

class IEventHandle: public INetObserver {
 public:
  int register_event(fd_t, EventType);
  int register_event(Socket::ISocket*, EventType);
  int shutdown_event(fd_t);
  int shutdown_event(Socket::ISocket*);
};

class IEvent {
 public:
  virtual ~IEvent() {}

  virtual int register_event(fd_t, IEventHandle*, EventType) = 0;
  virtual int shutdown_event(fd_t) = 0;
};

// Subject
class CEvent: public IEvent, public Thread::ITask {
 public:
  explicit CEvent(size_t);
  ~CEvent();

  int register_event(fd_t, IEventHandle*, EventType);
  int shutdown_event(fd_t);

 protected:
  void handle_task();  // ITask method

 private:
  enum ExistRet {
    NOT_EXIST,
    HANDLE_MODIFY,
    EXISTED,
  };

  enum Limit {
    EVENT_BUFF_LEN = 1024,
    COMMIT_AGAIN_NUM = 2,
  };

  typedef std::pair<fd_t, EventType> event_identify_t;
  typedef std::unordered_map<event_identify_t,
          std::shared_ptr<CNetObserver>, pair_hash> observer_map_t;
  typedef std::unordered_map<event_identify_t, bool, pair_hash> event_map_t;

 private:
  ExistRet is_exist(fd_t, EventType, IEventHandle*);
  int add_observer(fd_t, EventType, IEventHandle*);
  int remove_observer(fd_t fd, EventType type, bool release = false);
  std::shared_ptr<CNetObserver> get_observer(fd_t, EventType);

  int push_event_trigger(const struct kevent&);
  int pop_event_trigger(struct kevent&);

  size_t task_size();
  int erase_task();
  int unregister_event(fd_t, EventType);
  static void* event_wait_thread(void*);

 private:
  fd_t m_kqueue_fd;

  observer_map_t m_reg_event;
  std::mutex m_reg_event_mutex;

  std::queue<struct kevent> m_events;
  std::mutex m_events_mutex;
  event_map_t m_events_map;

  pthread_t m_detection_thread;
  struct kevent m_event_buffer[EVENT_BUFF_LEN];
  std::unique_ptr<Thread::IThreadpool> m_threadpool;
};

class CEventProxy: public IEvent {
 public:
  static CEventProxy* instance();

  int register_event(fd_t, IEventHandle*, EventType);
  int register_event(Socket::ISocket*, IEventHandle*, EventType);
  int shutdown_event(fd_t);
  int shutdown_event(Socket::ISocket*);

 private:
  explicit CEventProxy(size_t);
  ~CEventProxy();

 private:
  std::unique_ptr<CEvent> m_event;
};
}  // namespace Event
#endif  // cplint: NOLINT
