// simple_server_test.cpp
// These tests focus on logic surfaced in the PR diff: format_time %N support,
// error propagation via raise_errno, SimpleWidget template expansion, and ChainWidget validation.
//
// Testing framework note:
// - The tests auto-detect and use one of: Catch2 (v3/v2), GoogleTest, or doctest via __has_include.
// - If none are available in this repository, they fall back to a minimal assert-based harness.
// - This file is designed to append to existing test setups without introducing new dependencies.

#ifndef YU_SIMPLE_SERVER_TESTS_ADDED
#define YU_SIMPLE_SERVER_TESTS_ADDED 1

#include <string>
#include <unordered_map>
#include <chrono>
#include <cerrno>
#include <memory>
#include <iostream>

#if defined(__has_include)
  #if __has_include(<catch2/catch_test_macros.hpp>)
    #include <catch2/catch_test_macros.hpp>
    #define YU_TF_CATCH2_V3 1
  #elif __has_include(<catch2/catch.hpp>)
    #include <catch2/catch.hpp>
    #define YU_TF_CATCH2_V2 1
  #elif __has_include(<gtest/gtest.h>)
    #include <gtest/gtest.h>
    #define YU_TF_GTEST 1
  #elif __has_include(<doctest/doctest.h>)
    #include <doctest/doctest.h>
    #define YU_TF_DOCTEST 1
  #endif
#endif

#if \!defined(YU_TF_CATCH2_V3) && \!defined(YU_TF_CATCH2_V2) && \!defined(YU_TF_GTEST) && \!defined(YU_TF_DOCTEST)
  #include <cassert>
  #define YU_TF_ASSERT(expr) do { if(\!(expr)) { std::cerr << "Assertion failed: " #expr " at " << __FILE__ << ":" << __LINE__ << std::endl; std::abort(); } } while(0)
#endif

// Access private members for white-box testing where appropriate.
#define private public
#define protected public
#include "http/simple_server.hpp"
#undef private
#undef protected

// Unified assertion helpers across frameworks
#if defined(YU_TF_CATCH2_V3) || defined(YU_TF_CATCH2_V2) || defined(YU_TF_DOCTEST)
  #define YU_CHECK(expr) CHECK(expr)
  #define YU_CHECK_EQ(a,b) CHECK((a) == (b))
  #define YU_CHECK_THROWS_AS(stmt, ex) CHECK_THROWS_AS((stmt), ex)
#elif defined(YU_TF_GTEST)
  #define YU_CHECK(expr) EXPECT_TRUE((expr))
  #define YU_CHECK_EQ(a,b) EXPECT_EQ((a),(b))
  #define YU_CHECK_THROWS_AS(stmt, ex) EXPECT_THROW((stmt), ex)
#else
  #define YU_CHECK(expr) YU_TF_ASSERT((expr))
  #define YU_CHECK_EQ(a,b) YU_TF_ASSERT(((a) == (b)))
  #define YU_CHECK_THROWS_AS(stmt, ex) do { bool caught=false; try { (void)(stmt); } catch (const ex&) { caught=true; } YU_TF_ASSERT(caught); } while(0)
#endif

// ---------- Test bodies (framework-agnostic functions) ----------

static void test_detail_format_time_nanoseconds_only() {
  using namespace std::chrono;
  auto tp = system_clock::time_point{} + nanoseconds(123);
  const std::string out = yu::http::detail::format_time(tp, "%N");
  YU_CHECK_EQ(out, "000000123");
}

static void test_detail_format_time_multiple_tokens() {
  using namespace std::chrono;
  auto tp = system_clock::time_point{} + nanoseconds(987654321);
  const std::string out = yu::http::detail::format_time(tp, "ns=%N; copy=%N");
  // Both occurrences should be replaced identically
  size_t count = 0, pos = 0;
  const std::string needle = "987654321";
  while ((pos = out.find(needle, pos)) \!= std::string::npos) { ++count; pos += needle.size(); }
  YU_CHECK_EQ(count, static_cast<size_t>(2));
  YU_CHECK(out.find("ns=987654321; copy=987654321") \!= std::string::npos);
}

static void test_detail_format_time_zero_ns() {
  using namespace std::chrono;
  auto tp = system_clock::time_point{} + nanoseconds(0);
  const std::string out = yu::http::detail::format_time(tp, "%N");
  YU_CHECK_EQ(out, "000000000");
}

static void test_detail_raise_errno_propagates_code() {
  int saved = errno;
  errno = EINVAL;
  try {
    yu::http::detail::raise_errno("unit_test_call");
    YU_CHECK(false); // should not reach
  } catch (const std::system_error& e) {
    YU_CHECK_EQ(e.code().value(), EINVAL);
    // message should contain function name suffix " failed"
    const std::string msg = e.what();
    YU_CHECK(msg.find("unit_test_call") \!= std::string::npos);
  }
  errno = saved;
}

static void test_simple_widget_replace_template_basic() {
  yu::http::SimpleWidget w;
  std::unordered_map<std::string, std::string> vars{
    {"host", "example.com"},
    {"user", "alice"}
  };
  const std::string input = "Hello ${host}\! User=${user}.";
  const std::string expected = "Hello example.com\! User=alice.";
  const std::string out = w.replace_template(input, vars);
  YU_CHECK_EQ(out, expected);
}

static void test_simple_widget_replace_template_repeated_and_missing() {
  yu::http::SimpleWidget w;
  std::unordered_map<std::string, std::string> vars{
    {"host", "example.com"}
  };
  const std::string input = "${host}-${missing}-${host}";
  const std::string expected = "example.com-${missing}-example.com";
  const std::string out = w.replace_template(input, vars);
  YU_CHECK_EQ(out, expected);
}

static void test_simple_widget_replace_template_ipv6_value() {
  yu::http::SimpleWidget w;
  std::unordered_map<std::string, std::string> vars{
    {"host", "[2001:db8::1]"}
  };
  const std::string input = "http://${host}/path";
  const std::string expected = "http://[2001:db8::1]/path";
  const std::string out = w.replace_template(input, vars);
  YU_CHECK_EQ(out, expected);
}

static void test_chain_widget_constructor_nullptr_validation() {
  using yu::http::WidgetPtr;
  using yu::http::SimpleWidget;
  using yu::http::ChainWidget;

  WidgetPtr good = std::make_shared<SimpleWidget>();
  WidgetPtr none;

  // First null
  YU_CHECK_THROWS_AS(ChainWidget(none, good), std::invalid_argument);
  // Second null
  YU_CHECK_THROWS_AS(ChainWidget(good, none), std::invalid_argument);
}

static void test_operator_pipe_creates_chain_widget() {
  using yu::http::WidgetPtr;
  using yu::http::SimpleWidget;
  using yu::http::ChainWidget;

  WidgetPtr a = std::make_shared<SimpleWidget>();
  WidgetPtr b = std::make_shared<SimpleWidget>();
  WidgetPtr chained = (a | b);
  YU_CHECK(static_cast<bool>(chained));
  // Ensure the resulting shared_ptr actually points to a ChainWidget
  std::shared_ptr<ChainWidget> as_chain = std::dynamic_pointer_cast<ChainWidget>(chained);
  YU_CHECK(static_cast<bool>(as_chain));
}

static void test_simple_server_constants() {
  YU_CHECK_EQ(yu::http::SimpleServer::kBacklog, 5);
}

// ---------- Registration per framework ----------

#if defined(YU_TF_GTEST)

TEST(YuHttpSimpleServerDetailTest, FormatTimeNanosecondsOnly) { test_detail_format_time_nanoseconds_only(); }
TEST(YuHttpSimpleServerDetailTest, FormatTimeMultipleTokens) { test_detail_format_time_multiple_tokens(); }
TEST(YuHttpSimpleServerDetailTest, FormatTimeZeroNs) { test_detail_format_time_zero_ns(); }
TEST(YuHttpSimpleServerErrorTest, RaiseErrnoPropagatesCode) { test_detail_raise_errno_propagates_code(); }

TEST(YuHttpSimpleWidgetTest, ReplaceTemplateBasic) { test_simple_widget_replace_template_basic(); }
TEST(YuHttpSimpleWidgetTest, ReplaceTemplateRepeatedAndMissing) { test_simple_widget_replace_template_repeated_and_missing(); }
TEST(YuHttpSimpleWidgetTest, ReplaceTemplateIpv6Value) { test_simple_widget_replace_template_ipv6_value(); }

TEST(YuHttpChainWidgetTest, ConstructorNullptrValidation) { test_chain_widget_constructor_nullptr_validation(); }
TEST(YuHttpChainWidgetTest, OperatorPipeCreatesChainWidget) { test_operator_pipe_creates_chain_widget(); }

TEST(YuHttpSimpleServerConstTest, BacklogConstant) { test_simple_server_constants(); }

#elif defined(YU_TF_CATCH2_V3) || defined(YU_TF_CATCH2_V2) || defined(YU_TF_DOCTEST)

TEST_CASE("detail::format_time prints nanoseconds when using %N") { test_detail_format_time_nanoseconds_only(); }
TEST_CASE("detail::format_time replaces multiple %N occurrences") { test_detail_format_time_multiple_tokens(); }
TEST_CASE("detail::format_time with 0ns produces 000000000") { test_detail_format_time_zero_ns(); }
TEST_CASE("detail::raise_errno throws with errno propagated") { test_detail_raise_errno_propagates_code(); }

TEST_CASE("SimpleWidget::replace_template replaces basic placeholders") { test_simple_widget_replace_template_basic(); }
TEST_CASE("SimpleWidget::replace_template repeats known and preserves unknown") { test_simple_widget_replace_template_repeated_and_missing(); }
TEST_CASE("SimpleWidget::replace_template handles IPv6-style host values") { test_simple_widget_replace_template_ipv6_value(); }

TEST_CASE("ChainWidget throws on nullptr in constructor") { test_chain_widget_constructor_nullptr_validation(); }
TEST_CASE("operator| produces a ChainWidget instance") { test_operator_pipe_creates_chain_widget(); }

TEST_CASE("SimpleServer constants") { test_simple_server_constants(); }

#else
// Minimal fallback harness: run all tests sequentially
int main() {
  test_detail_format_time_nanoseconds_only();
  test_detail_format_time_multiple_tokens();
  test_detail_format_time_zero_ns();
  test_detail_raise_errno_propagates_code();

  test_simple_widget_replace_template_basic();
  test_simple_widget_replace_template_repeated_and_missing();
  test_simple_widget_replace_template_ipv6_value();

  test_chain_widget_constructor_nullptr_validation();
  test_operator_pipe_creates_chain_widget();

  test_simple_server_constants();

  std::cerr << "[INFO] simple_server tests (fallback harness) passed.\n";
  return 0;
}
#endif // framework selection

#endif // YU_SIMPLE_SERVER_TESTS_ADDED