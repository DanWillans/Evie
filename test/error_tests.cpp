#include "doctest/doctest.h"

#include "evie/error.h"

TEST_CASE("Error Tests")
{
  evie::Error err = evie::Error::OK();
  SUBCASE("Check Error is good") { REQUIRE(err.Good()); }
  SUBCASE("Check Error bool operator is equivalent to good") { REQUIRE(err); }
  SUBCASE("Check Error false bool operator is false") { REQUIRE_FALSE(!err); }
  SUBCASE("Check construction of an Error with a msg")
  {
    const char* err_msg = "Uh oh something bad happened";
    err = evie::Error(err_msg);
    REQUIRE(err.Bad());
    REQUIRE_EQ(err_msg, err.Message());
  }
}