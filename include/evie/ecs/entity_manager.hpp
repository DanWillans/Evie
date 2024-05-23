#ifndef INCLUE_ECS_ENTITY_MANAGER_H_
#define INCLUE_ECS_ENTITY_MANAGER_H_

#include <queue>

#include "evie/ids.h"
#include "evie/result.h"
#include "evie/core.h"

namespace evie {
class EVIE_API EntityManager
{
public:
  Result<EntityID> CreateEntity();
  void DestroyEntity(EntityID entity_id);
  [[nodiscard]] uint64_t EntityCount() const;

private:
  size_t entity_count_{ 1 };
  std::queue<size_t> free_slots_;
};
}// namespace evie

#endif