// Copyright [2023] <Kdimo>
#ifndef __PAIR_HASH_H__  // cplint: NOLINT
#define __PAIR_HASH_H__

#include <utility>
#include <functional>

struct pair_hash {
  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1, T2>& p) const {
     auto h1 = std::hash<T1>{}(p.first);
     auto h2 = std::hash<T2>{}(p.second);

     return h1 ^ h2;
  }
};

#endif  // cplint: NOLINT
