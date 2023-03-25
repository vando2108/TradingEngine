// Copyright [2023] <Kdimo>
#ifndef __SOCKET_H__  // cpplint: NOLINT
#define __SOCKET_H__

#include <memory>
#include <string>

namespace Socket {
class ISocket {
 public:
  virtual ~ISocket() {}
  virtual int fd() = 0;
};

class IClient: public ISocket {
 public:
  virtual ~IClient() {}

  virtual int start() = 0;
  virtual int close() = 0;
  virtual int set_nonblock(bool nonblock) = 0;
  virtual int connect(const std::string &addr, uint16_t port) = 0;
  virtual ssize_t recv(void* buf, size_t len, int flags) = 0;
  virtual ssize_t send(const void* buf, size_t len, int flags) = 0;
};

class IServer: public ISocket {
 public:
  virtual ~IServer() {}

  virtual int set_nonblock(bool block) = 0;
  virtual int start(size_t backlog) = 0;
  virtual int close() = 0;
  virtual bool is_close() = 0;
  virtual std::unique_ptr<IClient> accept() = 0;
};
}  // namespace Socket
#endif  // cpplint: NOLINT
