#ifndef EVIE_INCLUDE_TYPES_H_
#define EVIE_INCLUDE_TYPES_H_

#include <string>

#include <glm/glm.hpp>

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

// Vector types
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;

// Matrix types
using mat4 = glm::mat4;


}// namespace evie

#endif// ! EVIE_INCLUDE_TYPES_H_
