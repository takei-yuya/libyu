// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_DATA_HEAP_HPP_
#define YU_DATA_HEAP_HPP_

#include <vector>
#include <functional>

namespace yu {
namespace data {

template <typename T, typename Compare = std::less<T>, typename Allocator = std::allocator<T>>
class Heap {
 public:
  Heap() : heap_(), compare_() {}
  explicit Heap(const std::vector<T>& values)
      : heap_(values), compare_() {
    for (size_t i = heap_.size() / 2; i > 0; --i) {
      heap_down(i - 1);
    }
  }
  explicit Heap(std::vector<T>&& values)
      : heap_(std::move(values)), compare_() {
    for (size_t i = heap_.size() / 2; i > 0; --i) {
      heap_down(i - 1);
    }
  }

  Heap(const Heap&) = delete;
  Heap& operator=(const Heap&) = delete;

  Heap(Heap&&) = default;
  Heap& operator=(Heap&&) = default;

  void push(const T& value) {
    heap_.push_back(value);
    heap_up(heap_.size() - 1);
  }

  void push(T&& value) {
    heap_.push_back(std::move(value));
    heap_up(heap_.size() - 1);
  }

  template <class... Args>
  T& emplace(Args&&... args) {
    T& ret = heap_.emplace_back(std::forward<Args>(args)...);
    heap_up(heap_.size() - 1);
    return ret;
  }

  const T& top() const { return heap_.front(); }

  T pop() {
    T value = std::move(heap_.front());
    std::swap(heap_.front(), heap_.back());
    heap_.pop_back();
    heap_down(0);
    return value;
  }

  void drain(std::vector<T>& values) {
    values.reserve(values.size() + heap_.size());
    while (!empty()) {
      values.push_back(std::move(pop()));
    }
  }

  void clear() { heap_.clear(); }
  size_t size() const noexcept { return heap_.size(); }
  bool empty() const noexcept { return heap_.empty(); }
  size_t capacity() const noexcept { return heap_.capacity(); }
  void reserve(size_t size) { heap_.reserve(size); }

 private:
  void heap_up(size_t child) {
    if (child == 0) return;
    size_t parent = (child - 1) / 2;
    if (compare_(heap_[child], heap_[parent])) {
      std::swap(heap_[child], heap_[parent]);
      heap_up(parent);
    }
  }

  void heap_down(size_t parent) {
    size_t child = parent * 2 + 1;
    if (child >= heap_.size()) return;
    size_t right = child + 1;
    if (right < heap_.size() && compare_(heap_[right], heap_[child])) {
      child = right;
    }
    if (compare_(heap_[child], heap_[parent])) {
      std::swap(heap_[child], heap_[parent]);
      heap_down(child);
    }
  }

  std::vector<T, Allocator> heap_;
  Compare compare_;
};

}  // namespace data
}  // namespace yu

#endif  // YU_DATA_HEAP_HPP_
