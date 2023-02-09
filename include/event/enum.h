// Copyright [2023] <Kdimo>
#ifndef __ENUM_H__  // cplint: NOLINT
#define __ENUM_H__

#include <sys/event.h>

namespace Event {
enum EventConfig {
  NEVENT_MAX = 1024,
};

enum EventType {
  EV_READ = EVFILT_READ,
  EV_WRITE = EVFILT_WRITE,
  EV_ALL = EV_READ | EV_WRITE,
};

typedef int fd_t;
}  // namespace Event
#endif  // cplint: NOLINT
