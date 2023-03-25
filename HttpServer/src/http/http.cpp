// Copyright [2023] <Kdimo>
#include <string>
#include <iostream>

#include "../../include/util/util.h"
#include "../../include/http/http.h"

namespace Http {
  /*
   * CHttpRequest implement
   */
  CHttpRequest::CHttpRequest() {
    m_err = EMPTY_STRING;
  }

  CHttpRequest::~CHttpRequest() {}

  int CHttpRequest::load_packet(std::string_view packet) {
    int br_pos = packet.find(HTTP_BR);
    if (br_pos == std::string::npos) {
      set_error("startline not found");
      return -1;
    }

    std::string_view startline = packet.substr(0, br_pos);
    if (parse_startline(startline) < 0) {
      set_error("invalid startline");
      return -1;
    }

    int lpos = br_pos + 2;
    br_pos = packet.find("\r\n\r\n", lpos);
    std::string_view headerline = packet.substr(lpos, br_pos - lpos);

    if (parse_header(headerline) < 0) {
      set_error("failed to parse header");
      return -1;
    }

    lpos = br_pos + 4;
    m_body = packet.substr(lpos, packet.size());

    return 0;
  }

  std::string_view CHttpRequest::url() {
    return m_url;
  }

  std::string_view CHttpRequest::body() {
    return m_body;
  }

  std::string_view CHttpRequest::method() {
    return m_method;
  }

  std::string_view CHttpRequest::startline() {
    return m_startline;
  }

  std::string_view CHttpRequest::version() {
    return m_version;
  }

  const http_header_map_t& CHttpRequest::headers() {
    return m_headers;
  }

  bool CHttpRequest::has_header(const std::string& header) {
    return (m_headers.find(header) != m_headers.end());
  }

  std::string_view CHttpRequest::header_attr(const std::string& header) {
    auto it = m_headers.find(header);
    if (it == m_headers.end()) {
      return EMPTY_STRING;
    }

    return it->second;
  }

  void CHttpRequest::set_error(std::string_view err) {
    m_err = err;
  }

  int CHttpRequest::parse_startline(std::string_view startline) {
    auto split = str_split(startline, " ");
    if (split.size() != 3) {
      return -1;
    }

    m_method = split[0];
    m_url = split[1];
    m_version = split[2];

    return 0;
  }

  int CHttpRequest::parse_header(std::string_view headerline) {
    auto headers = str_split(headerline, HTTP_BR);

    for (auto it : headers) {
      auto split = str_split(it, ": ");
      if (split.size() != 2) {
        return -1;
      }

      m_headers[{split[0].begin(), split[0].end()}] =
        {split[1].begin(), split[1].end()};
    }

    return 0;
  }

  /*
   * CHttpResponse implement
   */
  CHttpResponse::CHttpResponse() {
    m_package.body = nullptr;
    m_package.body_len = 0;

    m_package.data = nullptr;
    m_package.data_len = 0;

    m_package.dirty = true;
  }

  CHttpResponse::~CHttpResponse() {
  }

  size_t CHttpResponse::size() {
    if (m_package.dirty) {
      m_package.total_size = startline_size() + headers_size();
      m_package.total_size += m_package.body_len;
    }

    return m_package.total_size;
  }

  const std::shared_ptr<char> CHttpResponse::serialize() {
    if (!m_package.dirty) {
      return m_package.data;
    }

    size_t total_size = size();

    std::shared_ptr<char> buffer(new char[total_size],
        smart_ptr_arr_del<char*>);
    assert(buffer != nullptr);

    char* buff = buffer.get();

    // version status reason
    int nprint = snprintf(buff, total_size, "%s %s %s\r\n",
        m_package.version.c_str(), m_package.status.c_str(), m_package.reason.c_str());  // cpplint: NOLINT
    if (nprint < 0) {
      return nullptr;
    }

    buff += nprint;
    total_size -= nprint;

    for (auto it : m_package.headers) {
      // name: attribute
      nprint = snprintf(buff, total_size, "%s: %s\r\n",
          it.first.c_str(), it.second.c_str());
      if (nprint < 0) {
        return nullptr;
      }

      buff += nprint;
      total_size -= nprint;
    }

    nprint = snprintf(buff, total_size, "\r\n");
    if (nprint < 0) {
      return nullptr;
    }

    buff += nprint;
    total_size -= nprint;

    memcpy(buff, m_package.body.get(), total_size);

    if (total_size != m_package.body_len) {
      error("failed to copy body, target=%ld, actually=%ld\n",
          m_package.body_len, total_size);
    }

    if (m_package.data != nullptr) {
      m_package.data.reset();
    }

    m_package.data = buffer;
    m_package.dirty = false;

    return m_package.data;
  }

  int CHttpResponse::set_status(const std::string& status,
      const std::string& reason) {
    m_package.status = status;
    m_package.reason = reason;
    m_package.dirty = true;

    return 0;
  }

  int CHttpResponse::set_body(const std::shared_ptr<char>& body, size_t len) {
    if (body == nullptr || len == 0 || len > BODY_MAX_SIZE) {
      return -1;
    }

    char* buffer = new char[len];
    assert(buffer != nullptr);

    memcpy(buffer, body.get(), len);

    if (m_package.body != nullptr) {
      m_package.body.reset();
    }

    m_package.body.reset(buffer, smart_ptr_arr_del<char*>);
    m_package.body_len = len;
    m_package.dirty = true;

    return 0;
  }

  int CHttpResponse::set_version(const std::string& version) {
    m_package.version = version;
    m_package.dirty = true;

    return 0;
  }

  int CHttpResponse::remove_header(const std::string& name) {
    auto it = m_package.headers.find(name);
    if (it != m_package.headers.end()) {
      m_package.headers.erase(it);
      m_package.dirty = true;

      return 0;
    } else {
      return -1;
    }
  }

  int CHttpResponse::add_header(const std::string& name, const std::string& attr) {  // cpplint: NOLINT
    if (name.empty() || attr.empty()) {
      return -1;
    }

    m_package.headers[name] = attr;
    m_package.dirty = true;

    return 0;
  }

  size_t CHttpResponse::startline_size() {
    const size_t other_char_size = 1 * 2 + 2;  // blank * 2 + CRLF
    size_t total_size = other_char_size + m_package.version.size();
    total_size += m_package.status.size() + m_package.reason.size();

    return total_size;
  }

  size_t CHttpResponse::headers_size() {
    const size_t other_char_size = 2 + 2;  // ': ' + CRLF
    const size_t head_terminatorsize = 2;  // CRLF

    size_t total_size = 0;
    for (auto it : m_package.headers) {
      total_size += it.first.size() + it.second.size() + other_char_size;
    }

    return (total_size += head_terminatorsize);
  }
}  // namespace Http
