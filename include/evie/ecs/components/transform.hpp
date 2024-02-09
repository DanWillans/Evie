#ifndef EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_
#define EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_

#include "evie/types.h"
namespace evie {
struct TransformRotationComponent
{
  evie::vec3 position{ 0.0 };
  evie::vec3 rotation{ 0.0 };
  evie::vec3 scale{ 1.0 };
};
}// namespace evie
#endif// !EVIE_INCLUDE_EVIE_ECS_COMPONENTS_TRANSFORM_HPP_
