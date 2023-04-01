// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_LIMITORDER_H_
#define ORDERBOOK_INCLUDE_LIMITORDER_H_

#include "./order.hpp"

namespace Orderbook {

class LimitOrder : public IOrder {
  friend class Orderbook;
 public:
  LimitOrder(std::string_view, Side, uint64_t, uint64_t);
  ~LimitOrder();

  inline OrderType type() {
    return OrderType::LIMIT_ORDER;
  }

  inline const Side& side() {
    return side_;
  }

  inline const uint64_t& limit() {
    return limit_;
  }

  inline const uint64_t& quantity() {
    return quantity_;
  }

 private:
  Side side_;

  uint64_t limit_;
  uint64_t quantity_;
};

}  // namespace Orderbook

#endif
