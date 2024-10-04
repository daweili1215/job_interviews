#pragma once

#include <iostream>
#include <cstring>
#include <memory>

template <typename T>
class singly_linked_list {
  struct node {
    T val;
    std::unique_ptr<node> next;
    node(T v_, std::unique_ptr<node> next_ = nullptr) : val{std::move(v_)}, next{std::move(next_)} {}
  };
  using node_ptr = std::unique_ptr<node>;

  node_ptr head;

  node_ptr reverse_helper(node_ptr current, node_ptr prev) {
      if (!current) {
          return prev; // return a new prev because current has been moved
      }

      node_ptr next = std::move(current->next);

      current->next = std::move(prev);

      return reverse_helper(std::move(next), std::move(current));
  }

public:

  void insert(T value) {
    auto new_node = std::make_unique<node>(value);
    if (!head) {
      head = std::move(new_node);
    } else {
      node* tmp = head.get();
      while (tmp->next) {
        tmp = tmp->next.get();
      }
      tmp->next = std::move(new_node);
    }
  }

  void iterative_reverse() {
    node_ptr prev = nullptr;
    node_ptr cur = std::move(head);
    node_ptr next;
    while (cur) {
      next = std::move(cur->next);
      cur->next = std::move(prev);
      prev = std::move(cur);
      cur = std::move(next);
    }
    head = std::move(prev);
  }

  void recursive_reverse() {
    head =  reverse_helper(std::move(head), nullptr);
  }

  void print() const {
    node* cur = head.get();
    while (cur) {
      std::cout << cur->val << " -> ";
      cur = cur->next.get();
    }
    std::cout << "nullptr" << std::endl;
  }
};