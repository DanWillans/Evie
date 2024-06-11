#include "evie/ecs/system.hpp"

namespace evie {
void System::UpdateSystem(const float& delta_time)
{
  // Call user implemented Update() function first
  Update(delta_time);

  // Iterate over entities marked for deletion and delete them.
  for (const auto& entity : entities_to_delete_) {
    entity.Destroy();
  }

  // Clear deleted entities for next iteration
  entities_to_delete_.clear();
}

void System::MarkEntityForDeletion(const Entity& entity) { entities_to_delete_.push_back(entity); }
}// namespace evie