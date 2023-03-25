// Copyright [2023] <Kdimo>
#include "../include/limit.hpp"
#include <cstdio>
#include <vector>

int main() {
  std::vector<int> a = {0, 1, 2, 3, 4};

  Orderbook::Limit::LimitTree limit_tree = Orderbook::Limit::LimitTree();
  for (auto it : a) {
    limit_tree.Insert(it);
  }

  limit_tree.Traverse();

  return 0;
}
