// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_ORDERBOOK_H_
#define ORDERBOOK_INCLUDE_ORDERBOOK_H_

#include "./limit.hpp"
#include "./limit_order.hpp"

#include <memory>
#include <algorithm>

namespace Orderbook {

class Orderbook {
  struct MatchingResult {
    uint64_t match_quantity;
  };

 public:
  Orderbook();
  ~Orderbook();

  void AddOrder(const std::shared_ptr<IOrder>&);

 private:
  inline uint64_t update_quantity(uint64_t* quantity_a, uint64_t* quantity_b) {
    uint64_t match_quantity  = std::min(*quantity_a, *quantity_b);
    quantity_a -= match_quantity;
    quantity_b -= match_quantity;

    return match_quantity;
  }

  MatchingResult limit(const std::shared_ptr<LimitOrder>& order);
  MatchingResult match_limit_order(const std::shared_ptr<Limit>&,
      const std::shared_ptr<LimitOrder>&,
      std::function<bool(uint64_t, uint64_t)>);

 private:
  LimitTree buy_tree_;
  LimitTree sell_tree_;

  std::shared_ptr<Limit> lowest_sell_;
  std::shared_ptr<Limit> highest_buy_;
};
}  // namespace Orderbook

#endif
