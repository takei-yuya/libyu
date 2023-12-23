#include "yu/data/heap.hpp"

#include "yu/test.hpp"

class HeapTest : public yu::Test {
};

TEST(HeapTest, testSimple) {
  yu::data::Heap<int> heap;
  EXPECT(true, ==, heap.empty());
  EXPECT(0, ==, heap.size());

  heap.push(3);

  EXPECT(false, ==, heap.empty());
  EXPECT(1, ==, heap.size());
  EXPECT(3, ==, heap.top());

  heap.push(1);

  EXPECT(false, ==, heap.empty());
  EXPECT(2, ==, heap.size());
  EXPECT(1, ==, heap.top());

  heap.push(2);

  EXPECT(false, ==, heap.empty());
  EXPECT(3, ==, heap.size());
  EXPECT(1, ==, heap.top());

  EXPECT(1, ==, heap.pop());

  EXPECT(false, ==, heap.empty());
  EXPECT(2, ==, heap.size());
  EXPECT(2, ==, heap.top());

  EXPECT(2, ==, heap.pop());

  EXPECT(false, ==, heap.empty());
  EXPECT(1, ==, heap.size());
  EXPECT(3, ==, heap.top());

  EXPECT(3, ==, heap.pop());

  EXPECT(true, ==, heap.empty());
  EXPECT(0, ==, heap.size());
}

TEST(HeapTest, testConstructor) {
  std::vector<int> v = {3, 1, 2};
  yu::data::Heap<int> heap(v);
  EXPECT(false, ==, heap.empty());
  EXPECT(3, ==, heap.size());
  EXPECT(1, ==, heap.top());
}

TEST(HeapTest, testClear) {
  yu::data::Heap<int> heap;
  heap.push(3);
  heap.push(1);
  heap.push(2);
  heap.clear();
  EXPECT(true, ==, heap.empty());
  EXPECT(0, ==, heap.size());
}

TEST(HeapTest, testMove) {
  yu::data::Heap<int> heap;
  heap.push(3);
  heap.push(1);
  heap.push(2);
  yu::data::Heap<int> heap2 = std::move(heap);
  EXPECT(false, ==, heap2.empty());
  EXPECT(3, ==, heap2.size());
  EXPECT(1, ==, heap2.top());
}

TEST(HeapTest, testSort) {
  std::vector<int> v = {3, 1, 2, 5, 4};
  yu::data::Heap<int> heap;
  for (auto i : v) {
    heap.push(i);
  }
  std::vector<int> actual;
  while (!heap.empty()) {
    actual.push_back(heap.pop());
  }
  std::vector<int> expected = {1, 2, 3, 4, 5};
  EXPECT(expected, ==, actual);
}

TEST(HeapTest, testSortReverse) {
  std::vector<int> v = {3, 1, 2, 5, 4};
  yu::data::Heap<int, std::greater<int>> heap;
  for (auto i : v) {
    heap.push(i);
  }
  std::vector<int> actual;
  while (!heap.empty()) {
    actual.push_back(heap.pop());
  }
  std::vector<int> expected = {5, 4, 3, 2, 1};
  EXPECT(expected, ==, actual);
}

class NonCopyable {
 public:
  explicit NonCopyable(int value) : value_(value) {}

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;

  int value() const { return value_; }

  bool operator<(const NonCopyable& other) const {
    return value_ < other.value_;
  }
  bool operator>(const NonCopyable& other) const {
    return value_ > other.value_;
  }

 private:
  int value_;
};

TEST(HeapTest, testNonCopyable) {
  yu::data::Heap<NonCopyable> heap;
  heap.push(NonCopyable(3));
  heap.emplace(1);
  heap.emplace(2);
  EXPECT(1, ==, heap.pop().value());
  EXPECT(2, ==, heap.pop().value());
  EXPECT(3, ==, heap.pop().value());
}

TEST(HeapTest, testNonCopyableMove) {
  yu::data::Heap<NonCopyable> heap;
  heap.push(NonCopyable(3));
  heap.emplace(1);
  heap.emplace(2);
  yu::data::Heap<NonCopyable> heap2 = std::move(heap);
  EXPECT(1, ==, heap2.pop().value());
  EXPECT(2, ==, heap2.pop().value());
  EXPECT(3, ==, heap2.pop().value());
}
