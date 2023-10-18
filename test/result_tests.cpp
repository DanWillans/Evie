#include <doctest/doctest.h>
#include <evie/logging.h>
#include <evie/result.h>

#include <initializer_list>
#include <memory>
#include <vector>

namespace {
class TestClass
{
public:
  TestClass(int a, double b, const char* c) : a(a), b(b), c(c) {}
  auto operator<=>(const TestClass&) const = default;
  int a;
  double b;
  const char* c;
};


class TestClassList
{
public:
  explicit TestClassList(std::initializer_list<int>&& list) : list(list) {}
  explicit TestClassList(const std::initializer_list<int>& list) : list(list) {}
  std::vector<int> list;
};

class TestClassPtr
{
public:
  explicit TestClassPtr(int i) : int_ptr(std::make_unique<int>(i)) {}
  TestClassPtr(TestClassPtr&& other) noexcept : int_ptr(std::move(other.int_ptr)) {}
  TestClassPtr& operator=(TestClassPtr&& other) noexcept
  {
    int_ptr = std::move(other.int_ptr);
    return *this;
  }
  TestClassPtr& operator=(const TestClassPtr& other) noexcept
  {
    int_ptr = std::make_unique<int>(*other.int_ptr);
    return *this;
  };

  std::unique_ptr<int> int_ptr;
};

}// namespace

TEST_CASE("Check error construction with l-value reference")
{
  evie::Error error("Uh oh, Error");
  evie::Result<TestClass> result{ error };
  REQUIRE(result.Bad());
  SUBCASE("Ensure Good() returns false after Bad() has returned true") { REQUIRE(!result.Good()); }
  REQUIRE(result.Error().Message() == error.Message());
}

TEST_CASE("Check error construction with r-value reference")
{
  evie::Result<TestClass> result{ "Uh oh, error!" };
  REQUIRE(result.Bad());
  REQUIRE(strcmp(result.Error().Message(), "Uh oh, error!") == 0);
}

TEST_CASE("Check result construction with const ResultType& argument")
{
  TestClass my_class{ 1, 2.0, "Test" };
  evie::Result<TestClass> result{ my_class };
  REQUIRE(result.Good());
  REQUIRE(result->a == my_class.a);
  REQUIRE(result->b == my_class.b);
  REQUIRE(result->c == my_class.c);
}

TEST_CASE("Check result construction with ResultType&& argument")
{
  evie::Result<TestClass> result{ TestClass{ 1, 2.0, "Test" } };
  REQUIRE(result.Good());
  REQUIRE(result->a == 1);
  REQUIRE(result->b == 2.0);
  REQUIRE(strcmp(result->c, "Test") == 0);
  // Construct with {} instead.
  evie::Result<TestClass> result_2{ 1, 2.0, "Test" };
  REQUIRE(result_2.Good());
  REQUIRE(result_2->a == 1);
  REQUIRE(result_2->b == 2.0);
  REQUIRE(strcmp(result_2->c, "Test") == 0);
}

TEST_CASE("Check result construction with initializer list construction for ResultType ")
{
  evie::Result<TestClassList> result{ std::initializer_list<int>{ 0, 1, 2, 3, 4 } };
  REQUIRE(result.Good());
  REQUIRE(result->list == std::vector{ 0, 1, 2, 3, 4 });
}

TEST_CASE("Check valid result copy constructor")
{
  evie::Result<TestClass> result{ 0, 1.0, "Test" };
  REQUIRE(result.Good());
  evie::Result<TestClass> copy_result{ result };
  REQUIRE(copy_result.Good());
  REQUIRE(result == copy_result);
}

TEST_CASE("Check error result copy constructor")
{
  evie::Result<TestClass> error_result{ "Uh oh, error!" };
  REQUIRE(error_result.Bad());
  evie::Result<TestClass> copy_error_result{ error_result };
  REQUIRE(copy_error_result.Bad());
  REQUIRE(error_result == copy_error_result);
}

TEST_CASE("Check valid result move constructor")
{
  evie::Result<TestClassPtr> result{ 1 };
  REQUIRE(result.Good());
  evie::Result<TestClassPtr> move_result{ std::move(result) };
  REQUIRE(move_result.Good());
  REQUIRE(!result->int_ptr);
  REQUIRE(move_result->int_ptr);
  REQUIRE(*move_result->int_ptr == 1);
}

TEST_CASE("Check error result move constructor")
{
  evie::Result<TestClass> error{ evie::Error{ "Uh oh, error!" } };
  REQUIRE(error.Bad());
  evie::Result<TestClass> move_error{ std::move(error) };
  REQUIRE(move_error.Bad());
}

TEST_CASE("Check error result move constructor")
{
  evie::Result<TestClass> error{ evie::Error{ "Uh oh, error!" } };
  REQUIRE(error.Bad());
  evie::Result<TestClass> move_error{ std::move(error) };
  REQUIRE(move_error.Bad());
}


TEST_CASE("Check Valid() returns true with valid result")
{
  evie::Result<TestClass> result{ 0, 1.0, "test" };
  REQUIRE(result.Good());
  TestClass* test_class_obj{ nullptr };
  if (result.Value(test_class_obj)) {
    REQUIRE(test_class_obj);
    REQUIRE(test_class_obj->a == 0);
    REQUIRE(test_class_obj->b == 1.0);
    REQUIRE(strcmp(test_class_obj->c, "test") == 0);
  }
}

TEST_CASE("Check Valid() returns true with error")
{
  evie::Result<TestClass> result{ "Uh oh, Error!" };
  REQUIRE(result.Bad());
  TestClass* test_class_obj{ nullptr };
  // Should return false because Result is in error.
  if (!result.Value(test_class_obj)) {
    // Should still be nullptr
    REQUIRE(!test_class_obj);
  }
}
