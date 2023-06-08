#include <doctest/doctest.h>

#include <Evie/sample_library.hpp>

TEST_CASE("Factorials are computed")
{
  REQUIRE(factorial(0) == 1);
  REQUIRE(factorial(1) == 1);
  REQUIRE(factorial(2) == 2);
  REQUIRE(factorial(3) == 6);
  REQUIRE(factorial(10) == 3628800);
}
