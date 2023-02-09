// Copyright [2023] <Kdimo>
#include <unistd.h>  // cpplint: NOLINT
#include <sys/socket.h>  // cpplint: NOLINT

#include "../../include/util/util.h"

#include "../../include/socket/client.h"
#include "../../include/socket/server.h"
#include "../../include/socket/sock-opt.h"

namespace Socket {
  CStreamServer::CStreamServer(const std::string& addr, uint16_t port):
    m_addr(addr), m_port(port), m_sockfd(-1) {}

  CStreamServer::~CStreamServer() {
    close();
  }

  int CStreamServer::fd() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    return m_sockfd;
  }

  int CStreamServer::set_nonblock(bool nonblock) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    return Socket::set_nonblock(m_sockfd, nonblock);
  }

  int CStreamServer::start(size_t backlog) {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (!INVALID_FD(m_sockfd)) {
      errsys("invalid socket fd\n");
      return -1;
    }

    struct sockaddr_in sockaddr;
    if (convert_inaddr(m_addr, m_port, sockaddr) < 0) {
      errsys("failed to convert socket address\n");
      return -1;
    }

    int sockfd = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      errsys("failed to create new socket\n");
      return -1;
    }

    assert(set_reuse_port(sockfd, true) == 0);

    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
      errsys("failed to bind socket\n");
      ::close(sockfd);
      return -1;
    }

    if (listen(sockfd, backlog) < 0) {
      errsys("failed to listen socket\n");
      ::close(sockfd);
      return -1;
    }

    m_sockfd = sockfd;
    return 0;
  }

  int CStreamServer::close() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return -1;
    }

    if (::close(m_sockfd) < 0) {
      return -1;
    }

    m_sockfd = -1;
    return 0;
  }

  bool CStreamServer::is_close() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return true;
    }
    return false;
  }

  std::unique_ptr<IClient> CStreamServer::accept() {
    std::lock_guard<std::mutex> guard(m_sockfd_mutex);
    if (INVALID_FD(m_sockfd)) {
      return nullptr;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    int clientfd = ::accept(m_sockfd, (struct sockaddr*)&client_addr,
        &client_addr_len);
    if (INVALID_FD(clientfd)) {
      return nullptr;
    }

    std::unique_ptr<IClient> client(new CStreamClient(clientfd));
    assert(client != nullptr);

    return client;
  }
}  // namespace Socket
