#ifndef EVIE_ECS_INCLUDE_SYSTEM_MANAGER_H_
#define EVIE_ECS_INCLUDE_SYSTEM_MANAGER_H_

#include <bitset>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "ecs_constants.hpp"
#include "evie/core.h"
#include "evie/ecs/component_manager.hpp"
#include "evie/ids.h"


namespace evie {

// Wrap a bitset but make it easier for users to use the ComponentID etc.
class SystemSignature
{
public:
  SystemSignature() = default;
  template<typename T> void SetComponent(ComponentID<T> component_id) { bitset_.set(component_id.Get()); }
  template<typename T> void ResetComponent(ComponentID<T> component_id) { bitset_.reset(component_id.Get()); }
  SystemSignature operator&(const SystemSignature& rhs) const { return SystemSignature{ rhs.bitset_ & bitset_ }; }
  bool operator==(const SystemSignature& rhs) const { return rhs.bitset_ == bitset_; }

private:
  explicit SystemSignature(const std::bitset<MAX_COMPONENT_COUNT>& bitset) : bitset_(bitset) {}
  std::bitset<MAX_COMPONENT_COUNT> bitset_;
};

// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
class System
{
public:
  std::unordered_set<EntityID> entities;
  SystemSignature signature;
  ComponentManager* component_manager;
};

// Responsible for managing systems.
// 1. You should be able to register a system and get a system id back.
// 2. You should be able to register components that the system requires.
// 3. It's not possible to re-register components that the system needs. This makes sense as a system generally
//    accesses just those components.

class SystemManager
{
public:
  explicit SystemManager(ComponentManager* component_manager) : component_manager_(component_manager) {}
  SystemManager(const SystemManager&) = delete;
  SystemManager& operator=(const SystemManager&) = delete;
  // Register a new system with a required signature.
  template<typename SystemName, typename... Args>
  [[nodiscard]] SystemID<SystemName> RegisterSystem(const SystemSignature& signature, Args... args)
  {
    SystemID<SystemName> system_id(systems_.size());
    const auto& system = systems_.emplace_back(std::make_unique<SystemName>(args...));
    system->signature = signature;
    system->component_manager = component_manager_;
    return system_id;
  }

  // Inform the systems that they no longer need to track this entity
  // as it's been destroyed
  void EVIE_API EntityDestroyed(EntityID entity_id) const;

  void EVIE_API EntitySignatureChanged(EntityID entity_id, const SystemSignature& new_entity_signature);

  template<typename SystemName> const System& GetSystem(SystemID<SystemName> system_id) const
  {
    return *systems_[system_id.Get()];
  }

  [[nodiscard]] SystemSignature& GetEntitySystemSignature(EntityID entity_id) { return signature_map_[entity_id]; }

private:
  std::vector<std::unique_ptr<System>> systems_;
  std::unordered_map<EntityID, SystemSignature> signature_map_;
  ComponentManager* component_manager_;
};

}// namespace evie

#endif// !EVIE_ECS_INCLUDE_SYSTEM_MANAGER_H_