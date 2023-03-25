// Copyright [2023] <Kdimo>
#pragma once

namespace Thread {
class ITask {
  friend class CThreadpool;

 public:
  virtual ~ITask() {}

 protected:
  virtual void handle_task() = 0;
};
}  // namespace Thread
