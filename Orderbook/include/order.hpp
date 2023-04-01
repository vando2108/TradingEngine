// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_ORDER_ORDER_H_
#define ORDERBOOK_INCLUDE_ORDER_ORDER_H_

#include <iostream>
#include <string>
#include <cstdint>

namespace Orderbook {

enum Side {
  Bid,
  Ask
};

enum OrderType {
  LIMIT_ORDER,
  MARKET_ORDER,
  ORDERBOOK_ENTRY,
};

class IOrder {
 public:
  explicit IOrder(std::string_view);
  virtual ~IOrder() = 0;

  virtual inline OrderType type() = 0;

 public:
  inline const uint64_t& id() const {
    return id_;
  }

  inline const std::string_view& user_id() {
    return user_id_;
  }

  inline const uint64_t& entry_time() {
    return entry_time_;
  }

 protected:
  uint64_t id_;
  std::string_view user_id_;
  uint64_t entry_time_;


 private:
  static uint64_t cnt_;
};

}  // namespace Orderbook

#endif
