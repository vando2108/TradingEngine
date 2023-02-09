// Copyright [2023] <Kdimo>
#ifndef __SERVER_H__  // cpplint: NOLINT
#define __SERVER_H__

#include <mutex>  // cpplint: NOLINT
#include <memory>
#include <string>
#include <fcntl.h>  // cpplint: NOLINT
#include <sys/socket.h>  // cpplint: NOLINT

#include "./socket.h"

namespace Socket {
class CStreamServer: public IServer {
 public:
  CStreamServer(const std::string&, uint16_t);
  ~CStreamServer();

  int fd();
  int set_nonblock(bool nonblock);
  int start(size_t backlog);
  int close();
  bool is_close();
  std::unique_ptr<IClient> accept();

 private:
  std::string m_addr;
  uint16_t m_port;

  int m_sockfd;
  std::mutex m_sockfd_mutex;
};
}  // namespace Socket
#endif  // cpplint: NOLINT
