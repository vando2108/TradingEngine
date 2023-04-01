// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_LIMIT_H_
#define ORDERBOOK_INCLUDE_LIMIT_H_

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <inttypes.h>

#include "./orderbook_entry.hpp"

namespace Orderbook {

class Limit {
  friend class LimitTree;
  friend class Orderbook;
 public:
  explicit Limit(
      const std::weak_ptr<Limit>&,
      const std::shared_ptr<OrderbookEntry>&);

  void log(std::string_view label = "");

 protected:
  // node value
  uint64_t limit_price_;
  uint64_t total_volume_;
  ListOrderbookEntry list_orderbook_entry_;

  uint8_t height_;
  std::weak_ptr<Limit> parent_;
  std::shared_ptr<Limit> left_child_;
  std::shared_ptr<Limit> right_child_;
};

class LimitTree {
 public:
  LimitTree();
  ~LimitTree();

 public:
  void Insert(const std::shared_ptr<OrderbookEntry>&);
  bool Remove(uint64_t);
  void Traverse();
  inline size_t size() {
    return node_map_.size();
  }

 private:
  void insert(
      const std::weak_ptr<Limit>&,
      std::shared_ptr<Limit>&,
      const std::shared_ptr<OrderbookEntry>&);
  void remove(const std::shared_ptr<Limit>&);

  std::shared_ptr<Limit> left_most_child(const std::shared_ptr<Limit>&);

  void traverse(std::shared_ptr<Limit>);

  uint8_t height(std::shared_ptr<Limit>);
  int get_balance(std::shared_ptr<Limit>);

  void insert_rebalance(uint64_t, std::shared_ptr<Limit>&);
  void remove_rebalance(std::shared_ptr<Limit>&);

  void swap_node(const std::shared_ptr<Limit>&, const std::shared_ptr<Limit>&);

  void left_rotate(std::shared_ptr<Limit>&);
  void right_rotate(std::shared_ptr<Limit>&);

  void update_height(std::shared_ptr<Limit>);

  void update_parent(
      const std::shared_ptr<Limit>& parent,
      const std::shared_ptr<Limit>& old_child,
      const std::shared_ptr<Limit>& new_child);

  void update_highest();
  void update_lowest();

 private:
  std::shared_ptr<Limit> root_;
  std::unordered_map<uint64_t, std::shared_ptr<Limit>> node_map_;

  std::shared_ptr<Limit> lowest_;
  std::shared_ptr<Limit> highest_;
};

}  // namespace Orderbook

#endif
