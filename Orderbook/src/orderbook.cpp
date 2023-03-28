// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDERBOOK_H_
#define ORDERBOOK_SRC_ORDERBOOK_H_

#include "../include/orderbook.hpp"
#include <memory>

namespace Orderbook {

Orderbook::Orderbook() : index_(0) {}
Orderbook::~Orderbook() {}

void Orderbook::AddOrder(const std::shared_ptr<IOrder>& order) {
  if (order->GetType() == OrderType::LIMIT_ORDER) {
    limit(std::dynamic_pointer_cast<LimitOrder>(order));
  }
}

Orderbook::MatchingResult Orderbook::limit(
    const std::shared_ptr<LimitOrder>& order) {
  uint64_t total_match = 0;
  if (order->is_buy_side_) {
    while (lowest_sell_ != nullptr
        && order->limit_ >= lowest_sell_->limit_price_
        && order->quantity_ > 0 ) {
      for (; lowest_sell_->list_orderbook_entry_.size();) {
        auto match_order = lowest_sell_->list_orderbook_entry_.head()->order_;
        // match_order can't null, because size is > 0
        assert(match_order != nullptr);

        total_match +=
          update_quantity(&order->quantity_, &match_order->quantity_);

        if (match_order->quantity_ == 0) {
          lowest_sell_->list_orderbook_entry_.RemoveOrder();
        }

        if (order->quantity_ == 0) {
          return {
            total_match,
          };
        }
      }
    }
  } else {
    while (highest_buy_ != nullptr
        && order->limit_ <= highest_buy_->limit_price_
        && order->quantity_ > 0) {
      for (; highest_buy_->list_orderbook_entry_.size();) {
        auto match_order = highest_buy_->list_orderbook_entry_.head()->order_;
        assert(match_order != nullptr);

        total_match +=
          update_quantity(&order->quantity_, &match_order->quantity_);

        if (match_order->quantity_ == 0) {
          highest_buy_->list_orderbook_entry_.RemoveOrder();
        }

        if (order->quantity_ == 0) {
          return {
            total_match,
          };
        }
      }
    }
  }

  return {0};
}

}  // namespace Orderbook

#endif
