// Copyright [2023] <Kdimo>
#include <fcntl.h>
#include <arpa/inet.h>
#include <iostream>

#include "../../include/socket/sock-opt.h"

namespace Socket {
  int set_reuse_port(int sockfd, bool en) {
    int enable = en? 0x01: 0x00;
    return setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,
        reinterpret_cast<void*>(&enable), sizeof(enable));
  }

  int set_nonblock(int sockfd, bool en) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (en) {
      flags |= O_NONBLOCK;
    } else {
      flags &= ~O_NONBLOCK;
    }

    return fcntl(sockfd, F_SETFL, flags);
  }

  int convert_inaddr(const std::string& addr, uint16_t port,
      struct sockaddr_in& sockaddr) {  // cpplint: NOLINT
    bzero(reinterpret_cast<void*>(&sockaddr), sizeof(sockaddr));

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    return 0;
  }
}  // namespace Socket
