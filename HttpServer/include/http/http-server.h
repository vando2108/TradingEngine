// Copyright [2023] <Kdimo>
#ifndef __HTTP_SERVER_H__  // cplint: NOLINT
#define __HTTP_SERVER_H__

#include <memory>
#include <string>

#include "./http.h"
#include "../event/event.hpp"
#include "../socket/server.h"

namespace Http {
class HttpStream: public Event::IEventHandle {
 public:
  explicit HttpStream(std::shared_ptr<Socket::IClient>);
  ~HttpStream();

  int close();

 protected:
  void handle_in(int);
  void handle_out(int);
  void handle_close(int);
  void handle_error(int);

 private:
  std::unique_ptr<Http::IHttpResponse> handle_request(Http::IHttpRequest&);

 private:
  const int READBUFF_LEN = 1024;

 private:
  std::shared_ptr<Socket::IClient> m_client;
  std::mutex  m_read_buffer_mutex;
  char* m_read_buffer;
};

class HttpServer: public Event::IEventHandle {
 public:
  HttpServer(const std::string&, uint16_t);
  ~HttpServer();

  int start(size_t);
  int close();

 protected:
  void handle_in(int);
  void handle_out(int);
  void handle_close(int);
  void handle_error(int);

 private:
  std::mutex m_sock_mutex;
  uint16_t m_port;
  const std::string m_addr;
  Socket::CStreamServer m_server;
};
}  // namespace Http
#endif  // cplint: NOLINT
