// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDER_RECORD_H_
#define ORDERBOOK_SRC_ORDER_RECORD_H_

#include "../include/orderbook_entry.hpp"
#include <memory>

namespace Orderbook {
OrderbookEntry::OrderbookEntry(const std::shared_ptr<LimitOrder>& order)
  : order_(order) {}

OrderbookEntry::~OrderbookEntry() {}

ListOrderbookEntry::ListOrderbookEntry()
  : size_(0), head_(nullptr), tail_(nullptr) {}

ListOrderbookEntry::~ListOrderbookEntry() {}

void ListOrderbookEntry::AddOrder(
    const std::shared_ptr<OrderbookEntry>& entry) {
  order_map_[entry->order_->id()] = entry;

  if (head_ == nullptr && tail_ == nullptr) {
    head_ = tail_ = entry;
    size_ = 1;
    return;
  }

  tail_->next_order_ = entry;
  entry->prev_order_ = tail_;

  tail_ = entry;
  ++size_;
}

/*
 * @param: id is order id, if id == 0 this function will remove first order
 * */
bool ListOrderbookEntry::RemoveOrder(uint64_t id) {
  if (id == 0) {
    if (size_ == 0) {
      return false;
    }

    if (size_ == 1) {
      head_ = tail_ = nullptr;
    } else {
      auto next = head_->next_order_;
      head_->next_order_ = nullptr;
      next->prev_order_ = nullptr;

      head_ = next;
    }

    return true;
  }

  auto order = order_map_.find(id);
  if (order == order_map_.end()) {
    return false;
  }

  // Remove from link-list
  auto cur = order->second;
  auto left = cur->prev_order_, right = cur->next_order_;

  left->next_order_ = right;
  right->prev_order_ = left;

  //  Remove from map
  order_map_.erase(order);
  --size_;

  return true;
}

}  // namespace Orderbook

#endif
