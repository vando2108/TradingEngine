// Copyright [2023] <Kdimo>
#ifndef ORDERBOOK_SRC_LIMIT_H_
#define ORDERBOOK_SRC_LIMIT_H_

#include <cstdio>
#include <memory>
#include <algorithm>
#include <inttypes.h>
#include <iostream>
#include <unordered_map>

#include "../include/limit.hpp"

namespace Orderbook {

Limit::Limit(
    const std::weak_ptr<Limit>& parent,
    const std::shared_ptr<OrderbookEntry>& entry)
  : limit_price_(entry->order()->limit()),
    total_volume_(entry->order()->quantity()),
    height_(1),
    parent_(parent),
    left_child_(nullptr),
    right_child_(nullptr) {
    list_orderbook_entry_.AddOrder(entry);
  }

void Limit::log(std::string_view label) {
  if (label != "") {
    std::cerr << label << '\n';
  }

  printf("-> cur %" PRIu64 "\n", limit_price_);
  auto parent = parent_.lock();
  if (parent) {
    printf("-> parent %" PRIu64 "\n", parent->limit_price_);
  } else {
    printf("-> parent is null\n");
  }

  if (left_child_) {
    printf("-> left %" PRIu64 "\n", left_child_->limit_price_);
  } else {
    printf("-> left is null\n");
  }

  if (right_child_) {
    printf("-> right %" PRIu64 "\n", right_child_->limit_price_);
  } else {
    printf("-> right is null\n");
  }
  printf("-> height %d\n\n", height_);
}

void LimitTree::swap_node(
  const std::shared_ptr<Limit>& a,
  const std::shared_ptr<Limit>& b
) {
  std::swap(a->limit_price_, b->limit_price_);
  std::swap(a->total_volume_, b->total_volume_);
  std::swap(a->list_orderbook_entry_, b->list_orderbook_entry_);

  node_map_[a->limit_price_] = a;
  node_map_[b->limit_price_] = b;
}

LimitTree::LimitTree()
  : root_(nullptr), lowest_(nullptr), highest_(nullptr) {}

LimitTree::~LimitTree() {}

void LimitTree::Insert(const std::shared_ptr<OrderbookEntry>& order) {
  insert(std::weak_ptr<Limit>(), root_, order);
}

bool LimitTree::Remove(uint64_t limit) {
  auto it = node_map_.find(limit);
  if (it == node_map_.end()) {
    return false;
  }
  remove(it->second);
  return true;
}

void LimitTree::remove(const std::shared_ptr<Limit>& node) {
  if (highest_ && highest_->limit_price_ == node->limit_price_) {
    update_highest();
  }

  if (lowest_ && lowest_->limit_price_ == node->limit_price_) {
    update_lowest();
  }

  auto parent = node->parent_.lock();

  if (node->left_child_ == nullptr || node->right_child_ == nullptr) {
    // node with only one or no child
    auto temp = node->left_child_ ? node->left_child_ : node->right_child_;

    if (temp == nullptr) {
      // no child
      update_parent(parent, node, nullptr);

      if (root_->limit_price_ == node->limit_price_) {
        root_ = nullptr;
      }
    } else {
      // one child
      update_parent(parent, node, temp);
      temp->parent_ = parent;

      if (root_->limit_price_ == node->limit_price_) {
        root_ = temp;
      }
    }

    // remove this node from map
    node_map_.erase(node->limit_price_);
    remove_rebalance(parent);
  } else {
    // two child
    auto temp = left_most_child(node->right_child_);
    swap_node(node, temp);

    remove(node);
  }

  return;
}

void LimitTree::insert(
    const std::weak_ptr<Limit>& parent,
    std::shared_ptr<Limit>& root,
    const std::shared_ptr<OrderbookEntry>& entry
) {
  if (root == nullptr) {
    root = std::make_shared<Limit>(parent, entry);
    node_map_[entry->order()->limit()] = root;

    if (highest_ == nullptr
      || entry->order()->limit() > highest_->limit_price_) {
      highest_ = root;
    }

    if (lowest_ == nullptr
      || entry->order()->limit() < lowest_->limit_price_) {
      lowest_ = root;
    }

    return;
  }

  if (entry->order()->limit() < root->limit_price_) {
    insert(root, root->left_child_, entry);
  } else if (entry->order()->limit() > root->limit_price_) {
    insert(root, root->right_child_, entry);
  } else {
    return;
  }

  update_height(root);
  insert_rebalance(entry->order()->limit(), root);
}

std::shared_ptr<Limit> LimitTree::left_most_child(
    const std::shared_ptr<Limit>& root
) {
  auto cur = root;
  while (cur->left_child_ != nullptr) {
    cur = cur->left_child_;
  }

  return cur;
}

void LimitTree::Traverse() {
  traverse(root_);
}

void LimitTree::traverse(std::shared_ptr<Limit> root) {
  if (root != nullptr) {
    root->log();
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

void LimitTree::insert_rebalance(uint64_t limit, std::shared_ptr<Limit>& root) {
  int balance = get_balance(root);

  if (balance > 1 && limit
      < root->left_child_->limit_price_) {
    right_rotate(root);
    return;
  }

  if (balance < -1 && limit
      > root->right_child_->limit_price_) {
    left_rotate(root);
    return;
  }

  if (balance > 1 && limit
      > root->left_child_->limit_price_) {
    left_rotate(root->left_child_);
    right_rotate(root);
    return;
  }

  if (balance < -1 && limit
      < root->right_child_->limit_price_) {
    right_rotate(root->right_child_);
    left_rotate(root);
    return;
  }
}

void LimitTree::remove_rebalance(std::shared_ptr<Limit>& root) {
  if (root == nullptr) {
    return;
  }

  update_height(root);
  int balance = get_balance(root);

  if (balance > 1) {
    if (get_balance(root->left_child_) >= 0) {
      // left left
      right_rotate(root);
    } else {
      // left right
      left_rotate(root->left_child_);
      right_rotate(root);
    }
  } else if (balance < -1) {
    if (get_balance(root->right_child_) <= 0) {
      // right right
      left_rotate(root);
    } else {
      // right left
      right_rotate(root->right_child_);
      left_rotate(root);
    }
  }

  if (!root->parent_.expired()) {
    auto parent = root->parent_.lock();
    remove_rebalance(parent);
  }
}

void LimitTree::left_rotate(std::shared_ptr<Limit>& x) {
  std::shared_ptr<Limit> y = x->right_child_;
  std::shared_ptr<Limit> T2 = y->left_child_;

  y->left_child_ = x;
  x->right_child_ = T2;

  update_height(x);
  update_height(y);

  if (root_ == x) {
    root_ = y;
  }
  x = y;
}

void LimitTree::right_rotate(std::shared_ptr<Limit>& y) {
  std::shared_ptr<Limit> x = y->left_child_;
  std::shared_ptr<Limit> T2 = x->right_child_;

  x->right_child_ = y;
  y->left_child_ = T2;

  update_height(y);
  update_height(x);

  if (root_ == y) {
    root_ = x;
  }
  y = x;
}

void LimitTree::update_height(std::shared_ptr<Limit> root) {
  root->height_ = std::max(height(root->left_child_),
      height(root->right_child_)) + 1;
}

void LimitTree::update_parent(
    const std::shared_ptr<Limit>& parent,
    const std::shared_ptr<Limit>& old_child,
    const std::shared_ptr<Limit>& new_child
) {
  if (parent == nullptr) {
    return;
  }

  if (parent->left_child_ == old_child) {
    parent->left_child_ = new_child;
  } else {
    parent->right_child_ = new_child;
  }
}

void LimitTree::update_highest() {
  if (highest_ == nullptr) {
    return;
  }

  if (highest_->left_child_) {
    highest_ = highest_->left_child_;
  } else {
    auto parent = highest_->parent_.lock();
    if (parent) {
      highest_ = parent;
    } else {
      highest_ = nullptr;
    }
  }
}

void LimitTree::update_lowest() {
  if (lowest_ == nullptr) {
    return;
  }

  if (lowest_->right_child_) {
    lowest_ = lowest_->right_child_;
  } else {
    auto parent = lowest_->parent_.lock();
    if (parent) {
      lowest_ = parent;
    } else {
      lowest_ = nullptr;
    }
  }
}

}  // namespace Orderbook

#endif
