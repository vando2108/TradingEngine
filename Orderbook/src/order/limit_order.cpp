// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDER_INTERACT_H_
#define ORDERBOOK_SRC_ORDER_INTERACT_H_

#include "../../include/order/limit_order.hpp"

namespace Orderbook {

LimitOrder::LimitOrder(std::string_view user_id,
    bool is_buy_side, uint64_t limit, uint64_t quantity)
  : is_buy_side_(is_buy_side), limit_(limit), quantity_(quantity),
    IOrder(user_id) {}

}  // namespace Orderbook

#endif
