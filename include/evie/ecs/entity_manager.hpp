#ifndef INCLUE_ECS_ENTITY_MANAGER_H_
#define INCLUE_ECS_ENTITY_MANAGER_H_

#include <queue>

#include "evie/core.h"
#include "evie/ids.h"
#include "evie/result.h"

namespace evie {
class EntityManager
{
public:
  Result<EntityID> EVIE_API CreateEntity();
  void EVIE_API DestroyEntity(EntityID entity_id);
  [[nodiscard]] uint64_t EVIE_API EntityCount() const;

private:
  size_t entity_count_{ 1 };
  std::queue<size_t> free_slots_;
};
}// namespace evie

#endif