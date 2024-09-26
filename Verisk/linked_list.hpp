#pragma once

#include <iostream>
#include <cstring>



/* problem 2
Write a template class that implements a single-linked list.
Write two methods of this class, both of which will reverse the order of the elements in the list. One
function should use a recursive approach; the second function should use an iterative approach.
For both functions, do not create a new list items; must instead re-use existing list items.
*/

template <typename T>
struct list_node {
  T val;
  list_node* next;
  list_node(T value) : val{value}, next{nullptr} {}
};

template <typename T>
class singly_linked_list {
  list_node<T>* head;

  list_node<T>* reverse_helper(list_node<T>* node) {
    if (!node || !node->next) {
      return node;
    }
    auto new_node = reverse_helper(node->next);
    node->next->next = node;
    node->next = nullptr;
    return new_node;
  }

public:
  singly_linked_list() : head{nullptr} {}

  ~singly_linked_list() {
    clean_up();
  }

  void clean_up() {
    while (head) {
      list_node<T>* tmp = head;
      head = head->next;
      delete tmp;
    }
  }

  void insert(T value) {
    auto node = new list_node<T>(value);
    if (!head) {
      head = node;
    } else {
      auto tmp = head;
      while (tmp->next) {
        tmp = tmp->next;
      }
      tmp->next = node;
    }
  }

  void iterative_reverse() {
    list_node<T>* node = nullptr;
    list_node<T>* cur = head;
    while (cur) {
      auto tmp = cur->next;
      cur->next = node;
      node = cur;
      cur = tmp;
    }
    head = node;
  }

  void recursive_reverse() {
    head = reverse_helper(head);
  }

  void print() const {
    list_node<T>* tmp = head;
    while (tmp) {
      std::cout << tmp->val << " -> ";
      tmp = tmp->next;
    }
    std::cout << "nullptr" << std::endl;
  }
};