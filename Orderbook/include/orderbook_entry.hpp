// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_ORDER_RECORD_H_
#define ORDERBOOK_INCLUDE_ORDER_RECORD_H_

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>

#include "./order.hpp"
#include "./limit_order.hpp"

namespace Orderbook {
class OrderbookEntry {
  friend class ListOrderbookEntry;
  friend class Orderbook;

 public:
  explicit OrderbookEntry(const std::shared_ptr<LimitOrder>&);
  ~OrderbookEntry();

 public:
  inline const std::shared_ptr<LimitOrder>& order() {
    return order_;
  }

 private:
  std::shared_ptr<LimitOrder> order_;

  std::shared_ptr<OrderbookEntry> next_order_;
  std::shared_ptr<OrderbookEntry> prev_order_;
};

class ListOrderbookEntry {
 public:
  ListOrderbookEntry();
  ~ListOrderbookEntry();

 public:
  inline const uint8_t& size() {
    return size_;
  }

  inline const std::shared_ptr<OrderbookEntry> head() {
    return head_;
  }

  void AddOrder(const std::shared_ptr<OrderbookEntry>&);
  bool RemoveOrder(uint64_t order_id = 0);
  void Pop();

 private:
  uint8_t size_;

  std::shared_ptr<OrderbookEntry> head_, tail_;
  std::unordered_map<uint64_t, std::shared_ptr<OrderbookEntry>> order_map_;
};

}  // namespace Orderbook

#endif
