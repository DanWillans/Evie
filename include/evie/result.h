#ifndef EVIE_INCLUDE_RESULT_H_
#define EVIE_INCLUDE_RESULT_H_

#include "evie/error.h"

#include <cassert>
#include <initializer_list>
#include <optional>
#include <string>
#include <type_traits>

#define assertm(exp, msg) assert(((void)msg, exp))

namespace evie {
template<typename ResultType, typename ErrorType>
concept move_assignable = std::is_move_assignable_v<ResultType> && std::is_move_assignable_v<ErrorType>;

template<typename ResultType, typename ErrorType>
concept copy_assignable = std::is_copy_assignable_v<ResultType> && std::is_copy_assignable_v<ErrorType>;

template<typename ResultType, typename ErrorType>
concept copy_constructible = std::is_copy_constructible_v<ResultType> && std::is_move_constructible_v<ErrorType>;

template<typename ResultType, typename ErrorType>
concept move_constructible = std::is_move_constructible_v<ResultType> && std::is_move_constructible_v<ErrorType>;

// This concept aims to avoid the variadic template constructors being picked
// over copy and move constructors for ResultType types.
template<typename ResultType, typename... Args>
concept exclude_result_type = (sizeof...(Args) != 1 && std::constructible_from<ResultType, Args...>)
                              || (sizeof...(Args) == 1 && std::constructible_from<ResultType, Args...>
                                  && !std::is_same_v<ResultType, std::decay_t<Args>...>);

// This concept aims to avoid the variadic template constructors being picked
// over copy and move constructors for ErrorType types.
template<typename ErrorType, typename... Args>
concept exclude_error_type = (sizeof...(Args) != 1 && std::constructible_from<ErrorType, Args...>)
                             || (sizeof...(Args) == 1 && std::constructible_from<ErrorType, Args...>
                                 && !std::is_same_v<ErrorType, std::decay_t<Args>...>);

// A Result class similar to Optional that can return ResultType or an
// ErrorType.
template<typename ResultType, typename ErrorType = Error> class Result
{
public:
  // ----------------------------------------
  // !! Construct a Result based on the ErrorType

  // R-value reference construction
  constexpr explicit Result(ErrorType&& error) : error_{ std::move(error) }, error_state_(true) {}

  // L-value reference construction
  constexpr explicit Result(const ErrorType& error) : error_{ error }, error_state_(true) {}
  // ----------------------------------------

  // ----------------------------------------
  // !! Construct a valid Result based on the ResultType

  // R-value reference construction
  constexpr explicit Result(ResultType&& result) : result_{ std::move(result) } {}

  // L-value reference construction
  constexpr explicit Result(const ResultType& result) : result_{ result } {}
  // ----------------------------------------

  // ----------------------------------------
  // Variadic template constructors
  template<typename... Args>
  // cppcheck-suppress noExplicitConstructor
  constexpr Result(Args&&... args)// NOLINT
    requires exclude_result_type<ResultType, Args...>
    : result_{ std::forward<Args>(args)... }
  {}

  template<typename... Args>
  // cppcheck-suppress noExplicitConstructor
  constexpr Result(Args&&... args)// NOLINT
    requires exclude_error_type<ErrorType, Args...>
    : error_{ std::forward<Args>(args)... }, error_state_(true)
  {}

  template<typename Type, typename... Args>
  constexpr Result(std::initializer_list<Type> list, Args&&... args)
    requires exclude_result_type<ResultType, Args...>
    : result_{ list, std::forward<Args>(args)... }
  {}

  template<typename Type, typename... Args>
  constexpr Result(std::initializer_list<Type> list, Args&&... args)
    requires exclude_error_type<ErrorType, Args...>
    : error_{ list, std::forward<Args>(args)... }, error_state_(true)
  {}
  // ----------------------------------------

  // ----------------------------------------
  // Copy constructor
  constexpr Result(const Result& result) noexcept
    requires copy_constructible<ResultType, ErrorType>
    : error_state_{ result.error_state_ }
  {
    if (error_state_) {
      new (&error_) ErrorType(result.error_);// NOLINT
    } else {
      new (&result_) ResultType(result.result_);// NOLINT
    }
  }
  // ----------------------------------------

  // ----------------------------------------
  // Move constructor
  constexpr Result(Result&& result) noexcept
    requires move_constructible<ResultType, ErrorType>
    : error_state_{ result.error_state_ }
  {
    if (error_state_) {
      new (&error_) ErrorType(std::move(result.error_));// NOLINT
    } else {
      new (&result_) ResultType(std::move(result.result_));// NOLINT
    }
  }
  // ----------------------------------------

  // ----------------------------------------
  // Destructor
  ~Result()
  {
    if (error_state_) {
      // cppcheck-suppress ignoredReturnValue
      error_.~ErrorType();// NOLINT
    } else {
      // cppcheck-suppress ignoredReturnValue
      result_.~ResultType();// NOLINT
    }
  }
  // ----------------------------------------

  // ----------------------------------------
  // Assignment operators
  // Copy assignment
  Result& operator=(const Result& result)
  {
    if (this == &result) {
      return *this;
    }
    error_state_ = result.error_state_;
    if (error_state_) {
      error_ = result.error_;// NOLINT
    } else {
      result_ = result.result_;// NOLINT
    }
    return *this;
  }

  // Move assignment
  Result& operator=(Result&& result) noexcept
  {
    if (this == &result) {
      return *this;
    }
    error_state_ = result.error_state_;
    if (error_state_) {
      error_ = std::move(result.error_);// NOLINT
    } else {
      result_ = std::move(result.result_);// NOLINT
    }
    return *this;
  }
  // ----------------------------------------

  // ----------------------------------------
  // Operator overloading
  constexpr bool operator==(const Result<ResultType, ErrorType>& rhs) const
  {
    if (rhs.error_state_ == error_state_) {
      if (error_state_) {
        return rhs.error_ == error_;// NOLINT
      } else {
        return rhs.result_ == result_;// NOLINT
      }
    } else {
      return false;
    }
  }

  constexpr explicit operator bool() const noexcept { return !error_state_; }

  /**
   * @brief Operator overload for dereferencing. This is not safe to call unless checking Good() returns true before
   * hand.
   *
   * @return constexpr ResultType& A reference to the ReturnType object.
   */
  [[nodiscard]] constexpr ResultType& operator*() noexcept
  {
    assertm(!error_state_, "Attempting to retrieve value when Result is in error!");
    return result_;// NOLINT
  }

  /**
   * @brief Operator overload for pointer dereferencing. This is not safe to call unless checking Good() return true
   * before hand.
   *
   * @return constexpr ResultType* A pointer to the ReturnType object.
   */
  [[nodiscard]] constexpr ResultType* operator->() noexcept
  {
    assertm(!error_state_, "Attempting to retrieve value when Result is in error!");
    return &result_;// NOLINT
  }
  // ----------------------------------------

  // ----------------------------------------
  // Helper functions

  /**
   * @brief Access the ResultType of the object. This is safe to call even if the object doesn't have a valid result.
   *
   * @param result A pointer reference to a ResultType. This will be filled in if the the function returns true
   * otherwise it will be untouched.
   * @return true If the object has a valid ResultType.
   * @return false If the object is in an error state and doesn't have a valid ResultType.
   */
  [[nodiscard]] constexpr bool Value(ResultType*& result) noexcept
  {
    if (error_state_) {
      return false;
    } else {
      result = &result_;// NOLINT
      return true;
    }
  }

  /**
   * @brief Return the ErrorType. This is not safe to call unless checking Bad() returns true before hand.
   *
   * @return constexpr const ErrorType& A reference to the ErrorType object.
   */
  [[nodiscard]] constexpr const ErrorType& Error() const noexcept
  {
    assertm(error_state_, "Attempting to retrieve error when Result is NOT in error!");
    return error_;// NOLINT
  }

  /**
   * @brief Signifies if the Result is good and has a valid ResultType.
   *
   * @return true If the object has a valid ResultType.
   * @return false If the object is in an error state and doesn't have a valid ResultType.
   */
  [[nodiscard]] constexpr bool Good() const noexcept { return !error_state_; }

  /**
   * @brief Signifies if the Result is bad and has a valid ErrorType.
   *
   * @return true If the object has a valid ErrorType.
   * @return false If the object is not in an error state and doesn't have a valid ErrorType.
   */

  [[nodiscard]] constexpr bool Bad() const noexcept { return error_state_; }

private:
  union {
    ResultType result_;
    ErrorType error_;
  };
  bool error_state_{ false };
};

}// namespace evie

#endif// !EVIE_INCLUDE_RESULT_H_