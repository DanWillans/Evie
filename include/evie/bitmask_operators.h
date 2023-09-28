#ifndef EVIE_UTILITY_BITMASK_OPERATORS_H_
#define EVIE_UTILITY_BITMASK_OPERATORS_H_

#include <type_traits>

namespace evie {
template<typename E> struct enable_bitmask_operators
{
  static constexpr bool enable = false;
};

template<typename EnumBitmaskType>
constexpr typename std::enable_if<enable_bitmask_operators<EnumBitmaskType>::enable, EnumBitmaskType>::type
  operator|(EnumBitmaskType a, EnumBitmaskType b)
{
  using type = typename std::underlying_type<EnumBitmaskType>::type;
  return static_cast<EnumBitmaskType>(static_cast<type>(a) | static_cast<type>(b));
}

template<typename EnumBitmaskType>
constexpr typename std::enable_if<enable_bitmask_operators<EnumBitmaskType>::enable, EnumBitmaskType>::type
  operator&(EnumBitmaskType a, EnumBitmaskType b)
{
  using type = typename std::underlying_type<EnumBitmaskType>::type;
  return static_cast<EnumBitmaskType>(static_cast<type>(a) & static_cast<type>(b));
}
}// namespace evie

#endif// !EVIE_UTILITY_BITMASK_OPERATORS_H_