#ifndef EVIE_ECS_INCLUDE_ENTITY_MANAGER_H_
#define EVIE_ECS_INCLUDE_ENTITY_MANAGER_H_

#include <cstdint>
#include <queue>


#include "evie/core.h"
#include "evie/ids.h"
#include "evie/result.h"


namespace evie {
// What is an entity?
// An entity is just EntityID type which is a concrete type wrapping an unsigned int. An entity doesn't hold any state.
//
// What is a component?
// A component encapsulates a specific piece of data e.g. audio clip, a transform, a mesh.
// A component can be associated with entities that require them.
//
// What is a system?
// A system operates on components of entities. For example a renderer may want to iterate over all entities that have a
// mesh component or a physics system mind want to iterate over all entities with mass and hitboxes.
//
// The EntityManager will handle creation of entities and applying components to entities

class EntityManager
{
public:
  Result<EntityID> EVIE_API CreateEntity();
  void EVIE_API DestroyEntity(EntityID entity_id);
  [[nodiscard]] uint64_t EVIE_API EntityCount() const;

private:
  uint64_t entity_count_{ 0 };
  std::queue<uint64_t> free_slots_;
};
}// namespace evie

#endif// !EVIE_ECS_INCLUDE_ENTITY_MANAGER_H_