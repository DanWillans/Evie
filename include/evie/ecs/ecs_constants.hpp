#ifndef EVIE_INCLUDE_ECS_INCLUDE_ECS_CONSTANTS_H_
#define EVIE_INCLUDE_ECS_INCLUDE_ECS_CONSTANTS_H_

#include <cstdint>
namespace evie {
// int32_max assigned to int64_t on purpose. Leaving room incase we need to expand. Do we really need int64_t max
// entities ever?!
constexpr int64_t MAX_ENTITY_COUNT = 100000;
constexpr int64_t MAX_COMPONENT_COUNT = 1024;
}// namespace evie

#endif// EVIE_INCLUDE_ECS_INCLUDE_ECS_CONSTANTS_H_