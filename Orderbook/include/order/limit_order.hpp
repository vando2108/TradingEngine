// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_ORDER_ORDERINTERACT_H_
#define ORDERBOOK_INCLUDE_ORDER_ORDERINTERACT_H_

#include "./order.hpp"

namespace Orderbook {

class LimitOrder : public IOrder {
  friend class Orderbook;
 public:
  LimitOrder(std::string_view, bool, uint64_t, uint64_t);
  ~LimitOrder();

  inline const bool& is_buy_side() {
    return is_buy_side_;
  }

  inline const uint64_t& limit() {
    return limit_;
  }

  inline const uint64_t& quantity() {
    return quantity_;
  }

 private:
  bool is_buy_side_;

  uint64_t limit_;
  uint64_t quantity_;
};

}  // namespace Orderbook

#endif
