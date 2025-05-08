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
  SizedQueue(SizedQueue&&) noexcept = default;
  SizedQueue& operator=(SizedQueue&&) noexcept = default;

  void Push(const T& data) {
    PushImpl(data);
  }

  void Push(T&& data) {
    PushImpl(std::move(data));
  }

  bool TryPush(const T& data) {
    return TryPushImpl(data);
  }

  bool TryPush(T&& data) {
    return TryPushImpl(std::move(data));
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

  bool Empty() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.empty();
  }

  bool Full() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size() >= capacity_;
  }

  size_t Size() const noexcept {
    std::lock_guard<std::mutex> lock(mutex_);
    return buffer_.size();
  }

 private:
  template <typename U>
  void PushImpl(U&& data) {
    std::unique_lock<std::mutex> lock(mutex_);
    full_cond_.wait(lock, [this]() { return buffer_.size() < capacity_; });
    buffer_.push_back(std::forward<U>(data));
    empty_cond_.notify_all();
  }

  template <typename U>
  bool TryPushImpl(U&& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (buffer_.size() >= capacity_) {
      return false;
    }
    buffer_.push_back(std::forward<U>(data));
    empty_cond_.notify_all();
    return true;
  }

  mutable std::mutex mutex_;
  std::condition_variable full_cond_;
  std::condition_variable empty_cond_;
  const size_t capacity_;
  std::deque<T, Allocator> buffer_;
};

}  // namespace data
}  // namespace yu

#endif  // YU_DATA_SIZED_QUEUE_HPP_
