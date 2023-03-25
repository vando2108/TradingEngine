// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_LIMIT_H_
#define ORDERBOOK_INCLUDE_LIMIT_H_

#include <cstdint>
#include <memory>

#include "../include/order.hpp"

namespace Orderbook {
namespace Limit {
class Limit {
  friend class LimitTree;
 public:
  explicit Limit(uint64_t limit_price)
    : height_(1), limit_price_(limit_price), total_volume_(0),
    left_child_(nullptr), right_child_(nullptr) {}
    // m_head_order(nullptr), m_tail_order(nullptr) {}

 protected:
  uint8_t height_;
  uint64_t limit_price_;
  uint64_t total_volume_;
  std::weak_ptr<Limit> parent_;
  std::shared_ptr<Limit> left_child_;
  std::shared_ptr<Limit> right_child_;
};

class LimitTree {
 public:
  LimitTree();
  ~LimitTree();
 public:
  void Insert(uint64_t);
  void Traverse();

 private:
  void insert(std::shared_ptr<Limit>&, uint64_t);
  void traverse(std::shared_ptr<Limit>);

  uint8_t height(std::shared_ptr<Limit>);
  int get_balance(std::shared_ptr<Limit>);
  void left_rotate(std::shared_ptr<Limit>&);
  void right_rotate(std::shared_ptr<Limit>&);
  void update_height(std::shared_ptr<Limit>);

 private:
  std::shared_ptr<Limit> root_;
};
}  // namespace Limit
}  // namespace Orderbook

#endif
