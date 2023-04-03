// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_ORDER_H_
#define ORDERBOOK_SRC_ORDER_H_

#include "../include/order.hpp"
#include <ctime>

namespace Orderbook {

uint64_t IOrder::cnt_ = 0;

IOrder::IOrder(std::string_view user_id)
  : id_(++cnt_), user_id_(user_id), entry_time_(std::time(0)) {}

IOrder::~IOrder() {}

}  // namespace Orderbook

#endif
