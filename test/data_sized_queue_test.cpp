#include "yu/data/sized_queue.hpp"

#include "yu/test.hpp"

#include <thread>

class SizedQueueTest : public yu::Test {
};

TEST(SizedQueueTest, testPushPop) {
  yu::data::SizedQueue<int> queue(10);
  queue.Push(1);
  queue.Push(2);
  queue.Push(3);
  EXPECT(1, ==, queue.Pop());
  EXPECT(2, ==, queue.Pop());
  EXPECT(3, ==, queue.Pop());
}

TEST(SizedQueueTest, testTryPushPop) {
  yu::data::SizedQueue<int> queue(3);
  EXPECT(true, ==, queue.TryPush(1));
  EXPECT(true, ==, queue.TryPush(2));
  EXPECT(true, ==, queue.TryPush(3));
  EXPECT(false, ==, queue.TryPush(4));
  EXPECT(3, ==, queue.Size());
  EXPECT(true, ==, queue.Full());

  int actual;
  EXPECT(true, ==, queue.TryPop(actual));
  EXPECT(1, ==, actual);
  EXPECT(true, ==, queue.TryPop(actual));
  EXPECT(2, ==, actual);
  EXPECT(true, ==, queue.TryPop(actual));
  EXPECT(3, ==, actual);
  EXPECT(false, ==, queue.TryPop(actual));
  EXPECT(0, ==, queue.Size());
  EXPECT(true, ==, queue.Empty());
}

TEST(SizedQueueTest, testWaitPushPop) {
  yu::data::SizedQueue<int> queue(3);
  std::thread t([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(3);
  });
  EXPECT(1, ==, queue.Pop());
  EXPECT(2, ==, queue.Pop());
  EXPECT(3, ==, queue.Pop());
  t.join();
}

TEST(SizedQueueTest, testWaitPushPopSmallCapacity) {
  yu::data::SizedQueue<int> queue(1);
  std::thread t([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.Push(3);
  });
  EXPECT(1, ==, queue.Pop());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT(2, ==, queue.Pop());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT(3, ==, queue.Pop());
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  t.join();
}

class NonCopyable {
 public:
  explicit NonCopyable(int value) : value_(value) {}
  ~NonCopyable() = default;

  // not copyable
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  // movable
  NonCopyable(NonCopyable&&) noexcept = default;
  NonCopyable& operator=(NonCopyable&&) noexcept = default;

  int value() const { return value_; }

 private:
  int value_;
};

TEST(SizedQueueTest, testPushPopNonCopyable) {
  yu::data::SizedQueue<NonCopyable> queue(10);
  queue.Push(NonCopyable(1));
  queue.Push(NonCopyable(2));
  queue.Push(NonCopyable(3));
  EXPECT(1, ==, queue.Pop().value());
  EXPECT(2, ==, queue.Pop().value());
  EXPECT(3, ==, queue.Pop().value());
}
