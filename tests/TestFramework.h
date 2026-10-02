#pragma once
// Minimal test harness (no external dependency): TEST(name) registers a case, CHECK/CHECK_NEAR assert.
#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace testing {

struct Case {
  std::string name;
  std::function<void()> body;
};

inline std::vector<Case>& registry() {
  static std::vector<Case> cases;
  return cases;
}

inline int& failures() {
  static int count = 0;
  return count;
}

struct Registrar {
  Registrar(std::string name, std::function<void()> body) { registry().push_back({std::move(name), std::move(body)}); }
};

inline void fail(const char* file, int line, const std::string& message) {
  ++failures();
  std::cerr << "    FAIL " << file << ":" << line << "  " << message << "\n";
}

}  // namespace testing

#define TEST(name)                                                    \
  static void test_##name();                                          \
  static const testing::Registrar registrar_##name(#name, test_##name); \
  static void test_##name()

#define CHECK(condition)                                              \
  do {                                                                \
    if (!(condition)) testing::fail(__FILE__, __LINE__, #condition);  \
  } while (0)

#define CHECK_EQ(actual, expected)                                                                          \
  do {                                                                                                      \
    const auto a_ = (actual);                                                                               \
    const auto e_ = (expected);                                                                             \
    if (!(a_ == e_))                                                                                        \
      testing::fail(__FILE__, __LINE__, std::string(#actual) + " == " + #expected + "  (got " + std::to_string(a_) + ")"); \
  } while (0)

#define CHECK_NEAR(actual, expected, tolerance)                                                              \
  do {                                                                                                       \
    const double a_ = (actual);                                                                              \
    const double e_ = (expected);                                                                            \
    if (std::fabs(a_ - e_) > (tolerance))                                                                    \
      testing::fail(__FILE__, __LINE__, std::string(#actual) + " ~= " + #expected + "  (got " + std::to_string(a_) + ")"); \
  } while (0)
