// Copyright [2023] <Kdimo>
#include <iostream>
#include "../../include/util/util.h"

std::vector<std::string_view> str_split(std::string_view s,
    std::string_view delimiter) {
  std::vector<std::string_view> ret;
  int pos = 0, lpos = 0;

  while ((pos = s.find(delimiter, pos + delimiter.size()))
      != std::string::npos) {
    ret.push_back(s.substr(lpos, pos - lpos));
    lpos = pos + delimiter.size();
  }
  ret.push_back(s.substr(lpos, s.size()));

  return ret;
}
