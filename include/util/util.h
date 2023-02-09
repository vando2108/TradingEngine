// Copyright [2023] <Kdimo>
#ifndef __UTIL_H__  // cplint: NOLINT
#define __UTIL_H__

#include <vector>
#include <string>

// Internal import
#include "./pair-hash.h"
#include "./param-check.h"
#include "./logger.h"

template<typename T>
void smart_ptr_arr_del(T ptr) {
  delete[] ptr;
}

inline void seterrno(int eno) {
  errno = eno;
}

std::vector<std::string_view> str_split(std::string_view s, std::string_view delimiter);  // cpplint: NOLINT

#endif  // cplint: NOLINT
