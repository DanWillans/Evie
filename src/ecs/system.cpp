#include "evie/ecs/system.hpp"

namespace evie {

ankerl::unordered_dense::set<Entity>* System::RegisterSystemSignature(const SystemSignature& signature)
{
  return &entity_sets.emplace_back(signature, ankerl::unordered_dense::set<Entity>{}).second;
}

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

void System::MarkEntityForDeletion(const Entity& entity) { entities_to_delete_.insert(entity); }
}// namespace evie