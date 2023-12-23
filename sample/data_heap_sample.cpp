#include "yu/data/heap.hpp"

#include <iostream>

int main() {
  std::vector<int> vec{5, 2, 6, 1, 3, 4};
  {
    yu::data::Heap<int> heap;
    for (auto i : vec) {
      heap.push(i);
    }
    std::cout << "asc: ";
    while (!heap.empty()) {
      std::cout << " " << heap.pop();
    }
    std::cout << std::endl;
  }
  {
    yu::data::Heap<int, std::greater<int>> heap;
    for (auto i : vec) {
      heap.push(i);
    }
    std::cout << "desc:";
    while (!heap.empty()) {
      std::cout << " " << heap.pop();
    }
    std::cout << std::endl;
  }
}
