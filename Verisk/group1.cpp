#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath> // for sqrt
#include "linked_list.hpp"

using namespace std;
// panel round, based on feedback, function func will be invoked 1B times, but only need to store the last 10 records, so use the circular array approach, less memory, 
// don't use % as it would over flow and expensive
// shared_ptr, unique_ptr related questions
constexpr int LIMIT = 10;
int records[LIMIT] = {0};
int counter = 0;
void func(int user_id) {
  if (counter >= LIMIT)
    counter = 0;
  records[counter++] = user_id;
}

void run_singly_linked_list_tests() {
  singly_linked_list<int> sll;
  sll.insert(1);
  sll.insert(2);
  sll.insert(3);
  sll.insert(4);
  std::cout << "singly linked list contents are: ";
  sll.print();
  sll.recursive_reverse();
  std::cout << "recursive_reverse invoked, singly linked list: ";
  sll.print();
  sll.iterative_reverse();
  std::cout << "iterative_reverse invoked, singly linked list: ";
  sll.print();

  return;
}

int main(int argc, char* argv[]) {
  /*
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <N> (where N is a non-negative integer)" << std::endl;
    return 1;
  }
  // assume the input fits INT_MAX, otherwise parsing and long long is needed
  int n = std::atoi(argv[1]);
  if (n < 0) {
    std::cout << "Error: N must be non-negetive" << std::endl;
    return 1;
  }
*/


  run_singly_linked_list_tests();

  /* problem 2
  A double-square number is an integer X which can be expressed as the sum of two perfect squares.
For example, 10 is a double-square because 10 = 3^2 + 1^2 (where the carat [^] represents
exponentiation). Given X, determine the number of ways in which it can be written as the sum of two
squares. For example, 10 can only be written as 3^2 + 1^2 (we don’t count 1^2 + 3^2 as being
different). On the other hand, 25 can be written as 5^2 + 0^2 or as 4^2 + 3^2.
Input
Create a command line executable called DoubleSquare.exe which takes a single parameter N and
returns the different ways N can be written as the sum of two squares.
  */
  int n = 25;
  auto res = std::vector<std::pair<int, int>>{};
  int left = 0, right = sqrt(n);
  while (left <= right) {
    auto sum = left * left + right * right;
    if (sum == n) {
      res.push_back({left, right});
      ++left;
      --right;
    } else if (sum < n) {
      ++left;
    } else {
      --right;
    }
  }
  if (!res.empty()) {
    for (auto& p : res)
      std::cout << n << " = " << p.first << "^2 + " << p.second << "^2" << std::endl;
  } else {
    std::cout << "No solution" << std::endl;
  }
  return 0;
}