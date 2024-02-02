#include "evie/ecs/component_manager.hpp"
#include "evie/ids.h"


namespace evie {

void ComponentManager::EntityDestroyed(EntityID entity_id)
{
  for (uint64_t i = 0; i < component_index_count_; ++i) {
    components_[i]->RemoveComponent(entity_id);
  }
}

}// namespace evie