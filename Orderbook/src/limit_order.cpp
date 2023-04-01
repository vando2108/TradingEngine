// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDER_INTERACT_H_
#define ORDERBOOK_SRC_ORDER_INTERACT_H_

#include "../include/limit_order.hpp"

namespace Orderbook {

LimitOrder::LimitOrder(std::string_view user_id,
    Side side, uint64_t limit, uint64_t quantity)
  : IOrder(user_id), side_(side), limit_(limit), quantity_(quantity) {}

LimitOrder::~LimitOrder() {}

}  // namespace Orderbook

#endif
