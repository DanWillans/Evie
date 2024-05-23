#include "evie/ecs/component_manager.hpp"

namespace evie {

void ComponentManager::EntityDestroyed(EntityID entity_id)
{
  for (size_t i = 0; i < component_index_count_; ++i) {
    components_[i]->RemoveComponent(entity_id);
  }
}
}// namespace evie