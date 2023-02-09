// Copyright [2023] <Kdimo>
#pragma once

#include <algorithm>
#include <vector>
#include <memory>
#include <mutex>  // cpplint: NOLINT
#include <stddef.h>  // cpplint: NOLINT
#include <pthread.h>  // cpplint: NOLINT

// Internal include
#include "./task.h"

#include "../../include/queue/queue.h"

namespace Thread {
class IThreadpool {
 public:
  virtual ~IThreadpool() {}

  /*
   * This function will be invoke to push a task to thread pool 
   * @return 0 if sucssess, negative value for error 
   * */
  virtual int push_task(ITask* task, bool block = true) = 0;
};

class CThreadpool: public IThreadpool {
 public:
  enum {
    THREADNUM_MAX = 10000,
    TASKNUM_MAX   = 100000,
  };

  // vector contains thread ids
  typedef std::vector<pthread_t> vector_tid_t;
  typedef Queue::CQueue<ITask*> task_queue_t;

 public:
  CThreadpool(size_t thread_size, size_t task_size);
  ~CThreadpool();
  int push_task(ITask* task, bool block);

 private:
  void promote_leader();
  void join_follower();
  void create();
  void destroy();
  static void* process_task(void* arg);

 private:
  int m_thread_num;
  vector_tid_t m_thread;
  task_queue_t m_task_queue;
  bool m_has_leader;
  std::condition_variable m_wait_leader;
  std::mutex m_indentify_mutex;
};

class CThreadpoolProxy: public IThreadpool {
 public:
  static IThreadpool* instance() {
    static CThreadpoolProxy threadpool_proxy;
    return &threadpool_proxy;
  }

  int push_task(ITask* task, bool block) {
    return m_it_threadpool->push_task(task, block);
  }

 private:
  CThreadpoolProxy() {
    m_it_threadpool.reset(new CThreadpool(1000, 100000));
  }

  ~CThreadpoolProxy() {
    m_it_threadpool.release();
  }

 private:
  std::unique_ptr<IThreadpool> m_it_threadpool;
};
}  // namespace Thread
