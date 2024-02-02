#include "evie/ecs/system_manager.hpp"
#include "evie/ids.h"

namespace evie {

void SystemManager::EntityDestroyed(EntityID entity_id) const
{
  // Iterate over all systems an inform them of the entity being removed.
  for (const auto& system : systems_) {
    system->entities.erase(entity_id);
  }
}
void SystemManager::EntitySignatureChanged(EntityID entity_id, const SystemSignature& new_entity_signature)
{
  signature_map_[entity_id] = new_entity_signature;
  for (const auto& system : systems_) {
    const auto& system_signature = system->signature;
    // If we AND the system_signature and the new_entity_signature
    // then we get the minimum set of similar components in a resultant signature.
    // This can then be compared with the system_signature again and if they are equal
    // then this system will want to know about this entity.
    // -------
    // Example
    // -------
    // system_signature     = 00001100
    // new_entity_signature = 00001111
    // (system_signature & new_entity_signature) = 00001100
    // The resultant signature equals the system signature so the system will want to know
    // about this entity
    if ((system_signature & new_entity_signature) == system_signature) {
      system->entities.insert(entity_id);
    } else {
      // It's safe to erase this entity from every system if it doesn't care about it
      // based on signature. Not the most optimised though.
      system->entities.erase(entity_id);
    }
  }
}

}// namespace evie