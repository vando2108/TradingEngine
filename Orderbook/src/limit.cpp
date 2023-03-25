// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_LIMIT_H_
#define ORDERBOOK_SRC_LIMIT_H_

#include <cstdio>
#include <memory>
#include <algorithm>
#include <inttypes.h>

#include "../include/limit.hpp"

namespace Orderbook {
namespace Limit {

LimitTree::LimitTree() {}
LimitTree::~LimitTree() {}

void LimitTree::Insert(uint64_t limit_price) {
  insert(root_, limit_price);
}

void LimitTree::insert(std::shared_ptr<Limit>& root, uint64_t limit_price) {
  if (root == nullptr) {
    root = std::make_shared<Limit>(limit_price);
    return;
  }

  if (limit_price < root->limit_price_) {
    insert(root->left_child_, limit_price);
  } else if (limit_price > root->limit_price_) {
    insert(root->right_child_, limit_price);
  } else {
    return;
  }

  update_height(root);
  int balance = get_balance(root);

  if (balance > 1 && limit_price < root->left_child_->limit_price_) {
    right_rotate(root);
    return;
  }

  if (balance < -1 && limit_price > root->right_child_->limit_price_) {
    left_rotate(root);
    return;
  }

  if (balance > 1 && limit_price > root->left_child_->limit_price_) {
    left_rotate(root->left_child_);
    right_rotate(root);
    return;
  }

  if (balance < -1 && limit_price < root->right_child_->limit_price_) {
    right_rotate(root->right_child_);
    left_rotate(root);
    return;
  }
}

void LimitTree::Traverse() {
  traverse(root_);
}

void LimitTree::traverse(std::shared_ptr<Limit> root) {
  if (root != nullptr) {
    printf("%" PRIu64 "\n", root->limit_price_);
    traverse(root->left_child_);
    traverse(root->right_child_);
  }
}

uint8_t LimitTree::height(std::shared_ptr<Limit> root) {
  if (root == nullptr) {
    return 0;
  }
  return root->height_;
}

int LimitTree::get_balance(std::shared_ptr<Limit> root) {
  if (root == nullptr) {
    return 0;
  }
  return height(root->left_child_) - height(root->right_child_);
}

void LimitTree::left_rotate(std::shared_ptr<Limit>& x) {
  std::shared_ptr<Limit> y = x->right_child_;
  std::shared_ptr<Limit> T2 = y->left_child_;

  y->left_child_ = x;
  x->right_child_ = T2;

  update_height(x);
  update_height(y);

  x = y;
}

void LimitTree::right_rotate(std::shared_ptr<Limit>& y) {
  std::shared_ptr<Limit> x = y->left_child_;
  std::shared_ptr<Limit> T2 = x->right_child_;

  x->right_child_ = y;
  y->left_child_ = T2;

  update_height(y);
  update_height(x);

  y = x;
}

void LimitTree::update_height(std::shared_ptr<Limit> root) {
  root->height_ = std::max(height(root->left_child_),
      height(root->right_child_)) + 1;
}

}  // namespace Limit
}  // namespace Orderbook

#endif
