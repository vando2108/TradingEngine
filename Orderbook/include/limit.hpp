// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_LIMIT_H_
#define ORDERBOOK_INCLUDE_LIMIT_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "./order/orderbook_entry.hpp"

namespace Orderbook {

class Limit {
  friend class LimitTree;
  friend class Orderbook;
 public:
  explicit Limit(const std::shared_ptr<OrderbookEntry>&);

 protected:
  uint8_t height_;
  uint64_t limit_price_;
  uint64_t total_volume_;
  std::weak_ptr<Limit> parent_;
  std::shared_ptr<Limit> left_child_;
  std::shared_ptr<Limit> right_child_;
  ListOrderbookEntry list_orderbook_entry_;
};

class LimitTree {
 public:
  LimitTree();
  ~LimitTree();
 public:
  void Insert(const std::shared_ptr<OrderbookEntry>&);
  void Traverse();

 private:
  void insert(std::shared_ptr<Limit>&, const std::shared_ptr<OrderbookEntry>&);
  void traverse(std::shared_ptr<Limit>);

  uint8_t height(std::shared_ptr<Limit>);
  int get_balance(std::shared_ptr<Limit>);
  void left_rotate(std::shared_ptr<Limit>&);
  void right_rotate(std::shared_ptr<Limit>&);
  void update_height(std::shared_ptr<Limit>);

 private:
  std::shared_ptr<Limit> root_;
  std::unordered_map<uint64_t, std::shared_ptr<Limit>> limit_map_;
};

}  // namespace Orderbook

#endif
