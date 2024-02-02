#ifndef EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_
#define EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_

#include "evie/types.h"
namespace evie {
struct TransformRotationComponent
{
  evie::vec3 position;
  evie::vec3 rotation;
  float scale;
};
}// namespace evie
#endif// !EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_
