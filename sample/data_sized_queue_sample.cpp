#include "yu/data/sized_queue.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

int main() {
  yu::data::SizedQueue<int> queue(5);
  std::thread t([&queue]() {
    for (int i = 0; i < 10; ++i) {
      queue.Push(i);
      std::ostringstream oss;
      oss << "<< " << i << std::endl;
      std::cout << oss.str();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  for (int i = 0; i < 10; ++i) {
    std::ostringstream oss;
    oss << ">> " << queue.Pop() << std::endl;
    std::cout << oss.str();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  t.join();
}
