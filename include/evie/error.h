#ifndef EVIE_ERROR_H_
#define EVIE_ERROR_H_

namespace evie {
class Error
{
public:
  explicit constexpr Error(const char* msg) : msg_(msg) {}

  // Static factory method to produce an OK error message
  [[nodiscard]] static constexpr Error OK() noexcept { return Error{}; }

  // Allow bool comparisons
  [[nodiscard]] explicit operator bool() const { return msg_ == nullptr; }

  // Check if the error is good
  [[nodiscard]] constexpr bool Good() const noexcept { return msg_ == nullptr; }

  // Check if the error is bad
  [[nodiscard]] constexpr bool Bad() const noexcept { return msg_ != nullptr; }

  // Get the error message
  // Consider putting __FILE__  __LINE__ stuff into this maybe?
  [[nodiscard]] constexpr const char* Message() const noexcept { return msg_; }

private:
  // No one should be able to construct a raw Error message unless using OK
  constexpr Error() : msg_(nullptr) {}
  const char* msg_;
};
}// namespace evie

#endif// EVIE_ERROR_H_