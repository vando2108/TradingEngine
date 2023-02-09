// Copyright [2023] <Kdimo>
#pragma once

#include <mutex> // cpplint: NOLINT
#include <queue>
#include <cstdint>
#include <iostream>

namespace Queue {
template<typename T> class IQueue {
 public:
  virtual ~IQueue() {}

  virtual size_t size() = 0;
  virtual int8_t clear() = 0;
  virtual int8_t pop(T* out) = 0;
  virtual int8_t pop_nonblock(T* out) = 0;
  virtual int8_t push(const T& in) = 0;
  virtual int8_t push_nonblock(const T& in) = 0;
};

template<typename T> class CQueue: public IQueue<T> {
 public:
  enum {
    MAX_WAIT_FREE = 10000,
    MAX_WAIT_FILL = 10000,
  };

  explicit CQueue(const size_t& len): m_max_queue_len(len) {
    m_num_wait_fill = 0;
    m_num_wait_free = 0;
  }

  ~CQueue() {}

  size_t size() {
    std::lock_guard<std::mutex> gurad(m_mutex);
    return m_queue.size();
  }

  int8_t clear() {
    std::lock_guard<std::mutex> guard(m_mutex);

    while (m_queue.size()) {
      m_queue.pop();
    }

    return 0;
  }

  int8_t pop(T* out) {
    std::unique_lock<std::mutex> locker(m_mutex);

    if (m_queue.empty()) {
      m_num_wait_fill++;
      m_cond_wait_fill.wait(locker, [&](){ return !m_queue.empty(); });
    }

    *out = m_queue.front();
    m_queue.pop();

    if (m_num_wait_free) {
      m_cond_wait_free.notify_one();
      m_num_wait_free--;
    }

    return 0;
  }

  int8_t pop_nonblock(T* out) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_queue.empty()) {
      return -1;
    }

    *out = m_queue.front();
    m_queue.pop();

    if (m_num_wait_free) {
      m_cond_wait_free.notify_one();
      m_num_wait_free--;
    }

    return 0;
  }

  int8_t push(const T& in) {
    std::unique_lock<std::mutex> locker(m_mutex);

    if (m_queue.size() == m_max_queue_len) {
      m_num_wait_free++;
      m_cond_wait_free.wait(locker,
        [&](){ return m_queue.size() < m_max_queue_len; });
    }

    m_queue.push(in);

    if (m_num_wait_fill) {
      m_cond_wait_fill.notify_one();
      m_num_wait_fill--;
    }

    return 0;
  }

  int8_t push_nonblock(const T& in) {
    std::lock_guard<std::mutex> guard(m_mutex);

    if (m_queue.size() >= m_max_queue_len) {
      return -1;
    }

    m_queue.push(in);

    if (m_num_wait_fill) {
      m_cond_wait_fill.notify_one();
      m_num_wait_fill--;
    }

    return 0;
  }

 private:
  typedef std::queue<T> queue_t;

  queue_t m_queue;
  std::mutex m_mutex;
  size_t m_num_wait_fill;
  size_t m_num_wait_free;
  std::condition_variable m_cond_wait_fill;
  std::condition_variable m_cond_wait_free;
  const size_t m_max_queue_len;
};
}  // namespace Queue
