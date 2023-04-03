// Copyright [2023] <Kdimo>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <glog/logging.h>

#include "../include/limit.hpp"
#include "../include/limit_order.hpp"

int main() {
  const std::string_view user_id = "user1";
  std::vector<uint64_t> prices = {44, 17, 62, 32, 50, 78, 48, 54, 88};
  Orderbook::LimitTree limit_tree;

  for (auto price : prices) {
    std::shared_ptr<Orderbook::LimitOrder> order(
        new Orderbook::LimitOrder(user_id, Orderbook::Side::Ask, price, 0));

    auto entry = std::make_shared<Orderbook::OrderbookEntry>(order);
    limit_tree.Insert(entry);
  }

  limit_tree.Remove(32);

  return 0;
}
