// Copyright [2023] <Kdimo>
#ifndef __DEFINE_H__  // cplint: NOLINT
#define __DEFINE_H__

#include <string>
#include <utility>
#include <unordered_map>

namespace Http {
  #define HTTP_VERSION "HTTP/1.1"

  #define HTTP_HEAD_CONTENT_TYPE "Content-type"
  #define HTTP_HEAD_CONTENT_LEN "Content-length"
  #define HTTP_HEAD_CONNECTION "Connection"

  #define HTTP_HEAD_KEEP_ALIVE "Keep-Alive"
  #define HTTP_ATTR_KEEP_ALIVE "keep-alive"

  #define HTTP_HEAD_HTML_TYPE "text/html"
  #define HTTP_HEAD_CSS_TYPE "text/css"
  #define HTTP_HEAD_GIF_TYPE "image/gif"
  #define HTTP_HEAD_JPG_TYPE "image/jpeg"
  #define HTTP_HEAD_PNG_TYPE "image/png"

  #define HTTP_BR "\r\n"

  #define EMPTY_STRING ""

  typedef std::pair<std::string, std::string> http_header_pair_t;
  typedef std::unordered_map<std::string, std::string> http_header_map_t;
}  // namespace Http
#endif  // cplint: NOLINT
