// Copyright [2023] <Kdimo>
#ifndef __SOCKOPT_H__  // cpplint: NOLINT
#define __SOCKOPT_H__

#include <string>
#include <stdint.h>  // cpplint: NOLINT
#include <netinet/in.h>  // cpplint: NOLINT

namespace Socket {
  int set_reuse_port(int sockfd, bool en);
  int set_nonblock(int sockfd, bool en);
  int convert_inaddr(const std::string&, uint16_t, struct sockaddr_in&);
}  // namespace Socket
#endif  // cpplint: NOLINT
