// Copyright [2023] <Kdimo>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../../include/util/util.h"
#include "../../include/http/http-server.h"

namespace Http {
  /*
   * HttpStream implement
   */
  HttpStream::HttpStream(std::shared_ptr<Socket::IClient> client) {
    m_client = client;

    m_read_buffer = new char[READBUFF_LEN + 1];
    assert(m_read_buffer != nullptr);

    m_client->set_nonblock(true);
    register_event(m_client.get(), Event::EV_READ);

    trace("constructor socket[%#X]\n", m_client->fd());
  }

  HttpStream::~HttpStream() {
    trace("destructor socket[%#X]\n", m_client->fd());

    delete []m_read_buffer;
    m_read_buffer = nullptr;
  }

  int HttpStream::close() {
    return shutdown_event(m_client.get());
  }

  void HttpStream::handle_in(int fd) {
    std::lock_guard<std::mutex> guard(m_read_buffer_mutex);
    ssize_t nread = m_client->recv(m_read_buffer, READBUFF_LEN, MSG_DONTWAIT);

    if (nread == 0 || (nread < 0 && errno != EAGAIN)) {
      close();
      return;
    } else if (nread < 0 && errno == EAGAIN) {
      return;
    }

    m_read_buffer[nread] = '\0';
    std::string string_request = m_read_buffer;

    Http::CHttpRequest http_request;
    if (http_request.load_packet(string_request) < 0) {
      error("failed to parse request\n");
      return;
    }
    trace("socket[%#X] received request\n", fd);

    std::unique_ptr<Http::IHttpResponse> response =
      handle_request(http_request);

    if (response != nullptr) {
      if (m_client->send(response->serialize().get(), response->size(), 0)
          < 0) {
        error("failed to send response to socket[%#X]\n", m_client->fd());
      } else {
        trace("send response to socket[%#X]\n", m_client->fd());
      }
    } else {
      handle_error(fd);
    }
  }

  void HttpStream::handle_out(int fd) {}

  void HttpStream::handle_close(int fd) {
    trace("socket[%#X] handle close\n", fd);
    delete this;
  }

  void HttpStream::handle_error(int fd) {
    close();
  }

  std::unique_ptr<Http::IHttpResponse> HttpStream::handle_request(Http::IHttpRequest& request) {  // cpplint: NOLINT
    // std::string_view url = request.url(), method = request.method();
    std::unique_ptr<Http::CHttpResponse> response(new Http::CHttpResponse());
    response->set_version(HTTP_VERSION);

    const std::string file_path =
      "/Users/kdimo2108/Workspace/Learn/httpserver-cpp/html/index.html";
    if (access(file_path.c_str(), R_OK) < 0) {
      errsys("file access error %s\n", file_path.c_str());

      response->set_status("404", "Not Found");
      response->add_header(HTTP_HEAD_CONNECTION, "close");

      return response;
    }

    struct stat file_stat;
    stat(file_path.c_str(), &file_stat);

    const size_t file_size = file_stat.st_size;

    std::shared_ptr<char> file_buffer(new char[file_size],
        smart_ptr_arr_del<char*>);
    assert(file_buffer != nullptr);

    FILE* file = fopen(file_path.c_str(), "rb");
    if (file == nullptr ||
        std::fread(file_buffer.get(), file_size, 1, file) != 0x01) {
      response->set_status("500", "Internal Server Error");
      response->add_header(HTTP_HEAD_CONNECTION, "close");

      return response;
    }
    fclose(file);

    char s_file_size[16] = {0x00};
    snprintf(s_file_size, sizeof(s_file_size), "%ld", file_size);

    response->set_status("200", "OK");
    response->add_header(HTTP_HEAD_CONTENT_TYPE, HTTP_HEAD_HTML_TYPE);
    response->add_header(HTTP_HEAD_CONTENT_LEN, s_file_size);
    response->add_header(HTTP_HEAD_CONNECTION, "close");
    response->set_body(file_buffer, file_size);

    return response;
  }

  /*
   * Http server implement
   */

  HttpServer::HttpServer(const std::string& addr, uint16_t port):
    m_port(port), m_addr(addr), m_server(addr, port) {
    trace("constructor addr %s:%d\n", addr.c_str(), port);
  }

  HttpServer::~HttpServer() {
    trace("destructor addr %s:%d\n", m_addr.c_str(), m_port);
  }

  int HttpServer::start(size_t backlog) {
    if (!m_server.is_close()) {
      return 0;
    }

    if (m_server.start(backlog) < 0) {
      return -1;
    }

    if (m_server.set_nonblock(true) < 0) {
      errsys("failed to set socket nonblock\n");
    }

    return register_event(&m_server, Event::EV_ALL);
  }

  int HttpServer::close() {
    return shutdown_event(&m_server);
  }

  void HttpServer::handle_in(int fd) {
    for (;;) {
      std::shared_ptr<Socket::IClient> conn = m_server.accept();

      if (conn == nullptr) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
          break;
        } else {
          close();
          errsys("sock fd=[%#X] accept error\n", fd);

          return;
        }
      }

      trace("socket [%#X] accept a connection\n", fd);
      HttpStream* http_stream = new HttpStream(conn);
      assert(http_stream != nullptr);
    }
  }

  void HttpServer::handle_out(int fd) {}

  void HttpServer::handle_close(int fd) {
    trace("socket [%#X] handle close\n", fd);
  }

  void HttpServer::handle_error(int fd) {
    close();
  }
}  // namespace Http
