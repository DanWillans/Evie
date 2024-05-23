#ifndef INCLUDE_ECS_SYSTEM_HPP_
#define INCLUDE_ECS_SYSTEM_HPP_

#include <unordered_map>

#include "component_manager.hpp"
#include "entity.hpp"
#include "evie/ids.h"
#include "system_signature.hpp"

#include "ankerl/unordered_dense.h"

namespace evie {
// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// It also includes some helper functions to the System.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
class System
{
public:
  ankerl::unordered_dense::set<Entity> entities;
  SystemSignature signature;
};
}// namespace evie

#endif// !INCLUDE_ECS_SYSTEM_HPP_