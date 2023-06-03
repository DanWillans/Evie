#include <doctest/doctest.h>

#include <Evie/sample_library.hpp>

TEST_CASE("Factorials are computed with constexpr")
{
  REQUIRE(factorial_constexpr(0) == 1);
  REQUIRE(factorial_constexpr(1) == 1);
  REQUIRE(factorial_constexpr(2) == 2);
  REQUIRE(factorial_constexpr(3) == 6);
  REQUIRE(factorial_constexpr(10) == 3628800);
}
