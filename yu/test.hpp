#ifndef YU_TEST_HPP_
#define YU_TEST_HPP_

#include "yu/json.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

namespace yu {

class TestFailure : public std::exception {
 public:
  TestFailure() {}
  virtual const char* what() const noexcept { return "Failure"; }
};

class Test {
 public:
  Test() : errors_() {}

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
  TestRunner() {}

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
    oss_ << __FILE__ ":" << __LINE__ << ": expect '" << yu::json::to_json(expected) << "' " #op " '" << yu::json::to_json(actual) << "', but not"; \
    errors_.push_back(oss_.str()); \
    throw yu::TestFailure(); \
  }

#define EXPECT_THROW(expression, exception_type) \
  try { \
    (expression); \
    std::ostringstream oss_; \
    oss_ << __FILE__ ":" << __LINE__ << ": expect throw " #exception_type ", but not"; \
    errors_.push_back(oss_.str()); \
  } catch (const exception_type& e) { \
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
    class_name ## _ ## test_name() { yu::TestRunner::getInstance().registerTest(#class_name "::" #test_name, this); } \
    virtual void run() { test_name(); } \
   private: \
    void test_name(); \
  }; \
  static class_name ## _ ## test_name class_name ## _ ## test_name ## _instance; \
  void class_name ## _ ## test_name :: test_name()

}  // namespace yu

int main() {
  return yu::TestRunner::getInstance().run() ? 0 : 1;
}
#endif  // YU_TEST_HPP_
