// Copyright [2023] <Kdimo>
#ifndef __CLIENT_H__  // cpplint: NOLINT
#define __CLIENT_H__

#include <mutex>  // cpplint: NOLINT
#include <string>

#include "./socket.h"

namespace Socket {
class CStreamClient: public IClient {
 public:
  CStreamClient();
  explicit CStreamClient(int);
  explicit CStreamClient(const CStreamClient&);
  ~CStreamClient();

  int fd();
  int start();
  int close();
  int set_nonblock(bool);
  int connect(const std::string&, uint16_t);
  ssize_t recv(void* buffer, size_t len, int flags = 0);
  ssize_t send(const void* buffer, size_t len, int flags = 0);

 private:
  int m_sockfd;
  std::mutex m_sockfd_mutex;
};
}  // namespace Socket
#endif  // cpplint: NOLINT
