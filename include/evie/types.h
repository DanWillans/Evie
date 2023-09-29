#ifndef EVIE_INCLUDE_TYPES_H_
#define EVIE_INCLUDE_TYPES_H_

#include <string>

namespace evie {
struct WindowDimensions
{
  int width{ 0 };
  int height{ 0 };
};
struct WindowProperties
{
  WindowDimensions dimensions;
  std::string name;
};
}// namespace evie

#endif// ! EVIE_INCLUDE_TYPES_H_
