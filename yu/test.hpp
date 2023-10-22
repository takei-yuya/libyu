// Copyright 2023 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_TEST_HPP_
#define YU_TEST_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "yu/json.hpp"

namespace yu {
namespace test {

class TestFailure : public std::exception {
 public:
  TestFailure() {}
  const char* what() const noexcept override { return "Failure"; }
};

class Test {
 public:
  Test() : errors_() {}
  virtual ~Test() {}

  virtual void run() = 0;
  virtual void prepare() {}
  virtual void teardown() {}
  bool ok() const { return errors_.empty(); }

  const std::vector<std::string> errors() const { return errors_; }

 protected:
  std::vector<std::string> errors_;
};

class TestRunner {
 private:
  TestRunner() : tests_() {}

 public:
  static TestRunner& getInstance() {
    static TestRunner runner;
    return runner;
  }

  void registerTest(const std::string& test_name, Test* test) {
    tests_.emplace_back(test_name, test);
  }

  bool run() {
    bool ok = true;
    for (auto& it : tests_) {
      Test* test = it.second;

      std::cout << "## " << it.first << ": ";
      std::cout.flush();
      test->prepare();
      try {
        it.second->run();
        if (!test->ok()) throw TestFailure();
        std::cout << "OK" << std::endl;
      } catch (const TestFailure& e) {
        ok = false;
        std::cout << e.what() << std::endl;
        for (const auto& error : test->errors()) {
          std::cout << error << std::endl;
        }
      } catch (const std::exception& e) {
        ok = false;
        std::cout << "FATAL" << std::endl;
        std::cout << "Uncaught exception: e = " << e.what() << std::endl;
        for (const auto& error : test->errors()) {
          std::cout << error << std::endl;
        }
      } catch (...) {
        ok = false;
        std::cout << "FATAL" << std::endl;
        std::cout << "Uncaught exception" << std::endl;
        for (const auto& error : test->errors()) {
          std::cout << error << std::endl;
        }
      }
      test->teardown();
    }
    return ok;
  }

 private:
  std::vector<std::pair<std::string, Test*>> tests_;
};

#define EXPECT(expected, op, actual) \
  if (!((expected) op (actual))) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": expect " << yu::json::to_json(expected) << " " #op " " << yu::json::to_json(actual) << ", but not"; \
    errors_.push_back(oss_.str()); \
  }

#define ASSERT(expected, op, actual) \
  if (!((expected) op (actual))) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": assert '" << yu::json::to_json(expected) << "' " #op " '" << yu::json::to_json(actual) << "', but not"; \
    errors_.push_back(oss_.str()); \
    throw yu::test::TestFailure(); \
  }

#define FAIL(msg) \
  { \
    errors_.push_back(msg); \
    throw yu::test::TestFailure(); \
  }

// TODO: move or redefine in another namespace like numeric, float_util or something else.
template <typename T>
int float_compare(T lhs, T rhs) {
  T epsilon = std::numeric_limits<T>::epsilon();
  if (lhs > rhs + epsilon) return 1;
  if (lhs + epsilon < rhs) return -1;
  return 0;
}

#define EXPECT_F(expected, op, actual) \
  if (!(yu::test::float_compare((expected), (actual)) op 0)) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": expect " << yu::json::to_json(expected) << " " #op " " << yu::json::to_json(actual) << ", but not"; \
    errors_.push_back(oss_.str()); \
  }

#define ASSERT_F(expected, op, actual) \
  if (!(yu::test::float_compare((expected), (actual)) op 0)) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": assert '" << yu::json::to_json(expected) << "' " #op " '" << yu::json::to_json(actual) << "', but not"; \
    errors_.push_back(oss_.str()); \
    throw yu::test::TestFailure(); \
  }

#define EXPECT_THROW(expression, exception_type) \
  try { \
    (expression); \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": expect throw " #exception_type ", but not"; \
    errors_.push_back(oss_.str()); \
  } catch (const exception_type&) { \
  } catch (const std::exception& e) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": except throw " #exception_type ", but aother exception is thrown, e = " << e.what(); \
    errors_.push_back(oss_.str()); \
  }

#define EXPECT_THROW_WHAT(expression, exception_type, expected_what) \
  try { \
    (expression); \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": expect throw " #exception_type ", but not"; \
    errors_.push_back(oss_.str()); \
  } catch (const exception_type& e) { \
    EXPECT(expected_what, ==, std::string(e.what())); \
  } catch (const std::exception& e) { \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": except throw " #exception_type ", but aother exception is thrown, e = " << e.what(); \
    errors_.push_back(oss_.str()); \
  }

#define TEST(class_name, test_name) \
  class class_name ## _ ## test_name : public class_name { \
   public: \
    class_name ## _ ## test_name() { yu::test::TestRunner::getInstance().registerTest(#class_name "::" #test_name, this); } \
    void run() override { test_name(); } \
   private: \
    void test_name(); \
  }; \
  static class_name ## _ ## test_name class_name ## _ ## test_name ## _instance; \
  void class_name ## _ ## test_name :: test_name()

}  // namespace test
using Test = test::Test;
}  // namespace yu

int main() {
  return yu::test::TestRunner::getInstance().run() ? 0 : 1;
}
#endif  // YU_TEST_HPP_
