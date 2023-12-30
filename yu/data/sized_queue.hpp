// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_DATA_SIZED_QUEUE_HPP_
#define YU_DATA_SIZED_QUEUE_HPP_

#include <condition_variable>
#include <mutex>
#include <deque>

namespace yu {
namespace data {

template <typename T, class Allocator = std::allocator<T>>
class SizedQueue {
 public:
  explicit SizedQueue(size_t capacity) : mutex_(), full_cond_(), empty_cond_(), capacity_(capacity), buffer_() {}
  ~SizedQueue() = default;

  // not copyable
  SizedQueue(const SizedQueue&) = delete;
  SizedQueue& operator=(const SizedQueue&) = delete;

  // movable
  SizedQueue(SizedQueue&&) = default;
  SizedQueue& operator=(SizedQueue&&) = default;

  void Push(const T& data) {
    std::unique_lock<std::mutex> lock(mutex_);
    full_cond_.wait(lock, [this]() { return buffer_.size() < capacity_; });
    buffer_.push_back(data);
    empty_cond_.notify_all();
  }

  void Push(T&& data) {
    std::unique_lock<std::mutex> lock(mutex_);
    full_cond_.wait(lock, [this]() { return buffer_.size() < capacity_; });
    buffer_.push_back(std::move(data));
    empty_cond_.notify_all();
  }

  bool TryPush(const T& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() >= capacity_) {
      return false;
    }
    buffer_.push_back(data);
    empty_cond_.notify_all();
    return true;
  }

  bool TryPush(T&& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() >= capacity_) {
      return false;
    }
    buffer_.push_back(std::move(data));
    empty_cond_.notify_all();
    return true;
  }

  T Pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    empty_cond_.wait(lock, [this]() { return !buffer_.empty(); });
    T data = std::move(buffer_.front());
    buffer_.pop_front();
    full_cond_.notify_all();
    return data;
  }

  bool TryPop(T& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.empty()) {
      return false;
    }
    data = std::move(buffer_.front());
    buffer_.pop_front();
    full_cond_.notify_all();
    return true;
  }

  bool Empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
  }

  bool Full() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size() >= capacity_;
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable full_cond_;
  std::condition_variable empty_cond_;
  const size_t capacity_;
  std::deque<T, Allocator> buffer_;
};

}  // namespace data
}  // namespace yu

#endif  // YU_DATA_SIZED_QUEUE_HPP_
