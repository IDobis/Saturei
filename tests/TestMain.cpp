#include "TestFramework.h"

int main() {
  for (const auto& test : testing::registry()) {
    const int before = testing::failures();
    test.body();
    std::cout << (testing::failures() == before ? "  ok    " : "  FAILED ") << test.name << "\n";
  }
  std::cout << "\n" << testing::registry().size() << " tests, " << testing::failures() << " failed assertion(s)\n";
  return testing::failures() == 0 ? 0 : 1;
}
