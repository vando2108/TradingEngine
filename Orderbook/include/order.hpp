// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_INCLUDE_ORDER_H_
#define ORDERBOOK_INCLUDE_ORDER_H_

#include <cstdint>
#include <memory>

namespace Orderbook {
namespace Order {

// class IOrder {
//  public:
//   IOrder() {}
// };
//
// class Order : public IOrder {
//  public:
//   Order(uint64_t id, bool is_buy_side, uint64_t quantity, uint64_t limit,
//       uint64_t entry_time)
//     : m_id(id), m_is_buy_side(is_buy_side), m_quantity(quantity),
//     m_limit(limit), m_entry_time(entry_time),
//     m_next_order(nullptr), m_prev_order(nullptr) {}
//
//  private:
//   uint64_t m_id;
//   bool m_is_buy_side;
//   uint64_t m_quantity;
//   uint64_t m_limit;
//   uint64_t m_entry_time;
//   std::unique_ptr<Order> m_next_order;
//   std::unique_ptr<Order> m_prev_order;
// };

}  // namespace Order
}  // namespace Orderbook

#endif
