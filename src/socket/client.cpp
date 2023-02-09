// Copyright [2023] <Kdimo>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../../include/util/util.h"

#include "../../include/socket/client.h"
#include "../../include/socket/sock-opt.h"

namespace Socket {
  CStreamClient::CStreamClient(): m_sockfd(-1) {}
  CStreamClient::CStreamClient(int fd): m_sockfd(fd) {}
  CStreamClient::CStreamClient(const CStreamClient& other) {
    m_sockfd = other.m_sockfd;
  }

  CStreamClient::~CStreamClient() {
    close();
  }

  int CStreamClient::fd() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    return m_sockfd;
  }

  int CStreamClient::start() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);

    if (!INVALID_FD(m_sockfd)) {
      return 0;
    }

    if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      return -1;
    }

    return 0;
  }

  int CStreamClient::close() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    int ret = ::close(m_sockfd);
    if (ret == 0x00) {
      m_sockfd = -1;
    }

    return ret;
  }

  int CStreamClient::set_nonblock(bool nonblock) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    return Socket::set_nonblock(m_sockfd, nonblock);
  }

  int CStreamClient::connect(const std::string& addr, uint16_t port) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    struct sockaddr_in sockaddr;
    bzero(reinterpret_cast<void*>(&sockaddr), sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);

    if (inet_aton(addr.c_str(), &sockaddr.sin_addr) < 0) {
      return -1;
    }

    if (::connect(m_sockfd, (struct sockaddr*)&sockaddr,
          sizeof(sockaddr)) < 0) {
      return -1;
    }

    return 0;
  }

  ssize_t CStreamClient::recv(void* buffer, size_t len, int flags) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    return ::recv(m_sockfd, buffer, len, 0);
  }

  ssize_t CStreamClient::send(const void* buffer, size_t len, int flags) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    size_t nsend = 0;
    ssize_t ret = 0;

    do {
      ret = ::send(m_sockfd, reinterpret_cast<const uint8_t*>(buffer) + nsend,
          len - nsend, 0);

      if (ret < 0) {
        if (errno != EINTR) {
          return ret;
        } else {
          continue;
        }
      }

      nsend += ret;
    } while (nsend != len);

    return nsend;
  }
}  // namespace Socket

