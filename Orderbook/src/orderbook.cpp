// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDERBOOK_H_
#define ORDERBOOK_SRC_ORDERBOOK_H_

#include "../include/orderbook.hpp"
#include <memory>

namespace Orderbook {

Orderbook::Orderbook() {}
Orderbook::~Orderbook() {}

void Orderbook::AddOrder(const std::shared_ptr<IOrder>& order) {
  if (order->type() == OrderType::LIMIT_ORDER) {
    limit(std::dynamic_pointer_cast<LimitOrder>(order));
  }
}

Orderbook::MatchingResult Orderbook::limit(
  const std::shared_ptr<LimitOrder>& order
) {
  uint64_t total_match = 0;
  if (order->side_ == Side::Ask) {
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

Orderbook::MatchingResult Orderbook::match_limit_order(
    const std::shared_ptr<Limit>& limit,
    const std::shared_ptr<LimitOrder>& order,
    std::function<bool(uint64_t, uint64_t)> cmp
) {
  uint64_t total_match = 0;

  while (limit != nullptr
      && cmp(order->limit(), limit->limit_price_)
      && order->quantity() > 0) {
    for (; limit->list_orderbook_entry_.size();) {
      auto match_order = limit->list_orderbook_entry_.head()->order_;
      assert(match_order != nullptr);

      total_match +=
        update_quantity(&order->quantity_, &match_order->quantity_);

      if (match_order->quantity_ == 0) {
        limit->list_orderbook_entry_.RemoveOrder();
      }

      if (order->quantity_ == 0) {
        return MatchingResult {
          total_match,
        };
      }
    }
  }

  // Add to orderbook
  if (order->quantity_ > 0) {
  }
}

}  // namespace Orderbook

#endif
