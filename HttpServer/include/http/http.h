// Copyright [2023] <Kdimo>
#ifndef __HTTP_H__  // cplint: NOLINT
#define __HTTP_H__

#include <memory>
#include <string>

#include "./define.h"

namespace Http {
class IHttpRequest {
 public:
  virtual ~IHttpRequest() {}

  virtual std::string_view url() = 0;
  virtual std::string_view body() = 0;
  virtual std::string_view method() = 0;
  virtual std::string_view startline() = 0;
  virtual std::string_view version() = 0;

  virtual const http_header_map_t& headers() = 0;
  virtual bool has_header(const std::string&) = 0;
  virtual std::string_view header_attr(const std::string&) = 0;
};

class IHttpResponse {
 public:
  virtual ~IHttpResponse() {}

  virtual size_t size() = 0;
  virtual const std::shared_ptr<char> serialize() = 0;

  virtual int set_version(const std::string&) = 0;
  virtual int set_body(const std::shared_ptr<char>&, size_t) = 0;
  virtual int set_status(const std::string&, const std::string&) = 0;

  virtual int add_header(const std::string&, const std::string&) = 0;
  virtual int remove_header(const std::string&) = 0;
};

class CHttpRequest: public IHttpRequest {
 public:
  CHttpRequest();
  ~CHttpRequest();

  int load_packet(std::string_view);
  std::string_view url();
  std::string_view body();
  std::string_view method();
  std::string_view startline();
  std::string_view version();

  const http_header_map_t& headers();
  bool has_header(const std::string&);
  std::string_view header_attr(const std::string&);

 private:
  inline void set_error(std::string_view);
  int parse_startline(std::string_view);
  int parse_header(std::string_view);

 private:
  std::string_view m_url;
  std::string_view m_err;
  std::string_view m_body;
  std::string_view m_method;
  std::string_view m_version;
  std::string_view m_startline;
  http_header_map_t m_headers;
};

class CHttpResponse: public IHttpResponse {
 public:
  CHttpResponse();
  ~CHttpResponse();

  size_t size();
  const std::shared_ptr<char> serialize();

  int set_status(const std::string&, const std::string&);
  int set_body(const std::shared_ptr<char>&, size_t);
  int set_version(const std::string&);

  int remove_header(const std::string&);
  int add_header(const std::string&, const std::string&);

 private:
  size_t startline_size();
  size_t headers_size();

 private:
  const size_t MAX_LINE = 1024;
  const size_t BODY_MAX_SIZE = 64 * MAX_LINE;

  struct response_packge_t {
    std::string reason;
    std::string status;
    std::string version;
    http_header_map_t headers;

    std::shared_ptr<char> body;
    size_t body_len;

    std::shared_ptr<char> data;
    size_t data_len;

    size_t total_size;
    bool dirty;
  };

  struct response_packge_t m_package;
};
}  // namespace Http
#endif  // cplint: NOLINT
