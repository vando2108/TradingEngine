// Copyright [2023] <Kdimo>
#include <pthread.h>

#include "../../include/thread/thread.h"

#include "../../include/util/util.h"

namespace Thread {
  CThreadpool::CThreadpool(size_t thread_size, size_t task_size):
    m_task_queue(task_size) {
    m_thread_num = std::min(thread_size, size_t(THREADNUM_MAX));

    create();
  }

  CThreadpool::~CThreadpool() {
    destroy();
  }

  void CThreadpool::promote_leader() {
    std::unique_lock<std::mutex> locker(m_indentify_mutex);
    if (m_has_leader) {
      m_wait_leader.wait(locker, [&](){ return !m_has_leader; });
    }
    m_has_leader = true;
  }

  void CThreadpool::join_follower() {
    std::lock_guard<std::mutex> guard(m_indentify_mutex);
    m_has_leader = false;
    m_wait_leader.notify_one();
  }

  int CThreadpool::push_task(ITask* task, bool block = true) {
    if (block) {
      return m_task_queue.push(task);
    } else {
      return m_task_queue.push_nonblock(task);
    }
  }

  void* CThreadpool::process_task(void* arg) {
    CThreadpool &threadpool = *reinterpret_cast<CThreadpool*>(arg);

    while (true) {
      threadpool.promote_leader();
      ITask* task = nullptr;
      int8_t ret = threadpool.m_task_queue.pop(&task);
      threadpool.join_follower();

      if (ret == 0 && task != nullptr) {
        task->handle_task();
      }
    }

    pthread_exit(nullptr);
  }

  void CThreadpool::create() {
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    for (int i = 0; i < m_thread_num; i++) {
      pthread_t tid = 0;

      if (pthread_create(&tid, &thread_attr, process_task,
            reinterpret_cast<void*>(this)) < 0) {
        errsys("Create thread[%d] failed\n", i);
        continue;
      }

      m_thread.push_back(tid);
    }

    pthread_attr_destroy(&thread_attr);
    trace("created threadpool, number of thread %d\n",
        static_cast<int>(m_thread.size()));
  }

  void CThreadpool::destroy() {
    void* ret_value = nullptr;
    for (auto thread : m_thread) {
      if (pthread_cancel(thread) < 0 || pthread_join(thread, &ret_value) < 0) {
        //errsys("destroy thread[%d]\n", *thread);
        continue;
      }
    }

    m_thread.clear();
    trace("destroy threadpool successful");
  }
}  // namespace Thread
