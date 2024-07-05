
#ifndef INCLUDE_SYSTEM_MANAGER_H_
#define INCLUDE_SYSTEM_MANAGER_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "ankerl/unordered_dense.h"

#include "component_manager.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "evie/core.h"
#include "evie/ids.h"
#include "system.hpp"
#include "system_manager_interface.hpp"


namespace evie {

// Responsible for managing systems.
// 1. You should be able to register a system and get a system id back.
// 2. You should be able to register components that the system requires.
// 3. It's not possible to re-register components that the system needs. This makes sense as a system generally
//    accesses just those components.

class EVIE_API SystemManager : public ISystemManager
{
public:
  SystemManager(ComponentManager* component_manager, EntityManager* entity_manager)
    : component_manager_(component_manager), entity_manager_(entity_manager)
  {}
  SystemManager(const SystemManager&) = delete;
  SystemManager(SystemManager&&) = delete;
  SystemManager& operator=(const SystemManager&) = delete;
  SystemManager& operator=(SystemManager&&) = delete;
  virtual ~SystemManager() = default;
  // Register a new system with a required signature.
  template<typename SystemName, typename... Args>
  [[nodiscard]] SystemID<SystemName> RegisterSystem(const SystemSignature& signature, Args... args)
  {
    SystemID<SystemName> system_id(systems_.size());
    const auto& system = systems_.emplace_back(std::make_unique<SystemName>(args...));
    system->signature = signature;
    system->component_manager = component_manager_;
    system->system_manager = this;
    auto res = system->RegisterSystemSignature(signature);
    assert(res);
    return system_id;
  }

  // Inform the systems that they no longer need to track this entity
  // as it's been destroyed
  void EntityDestroyed(EntityID entity_id) override;

  void EntitySignatureChanged(EntityID entity_id, const SystemSignature& new_entity_signature) override;

  template<typename SystemName> SystemName& GetSystem(SystemID<SystemName> system_id) const
  {
    return *static_cast<SystemName*>(systems_[static_cast<size_t>(system_id.Get())].get());
  }

  [[nodiscard]] SystemSignature& GetEntitySystemSignature(EntityID entity_id) override
  {
    return signature_map_[entity_id];
  }

private:
  std::vector<std::unique_ptr<System>> systems_;
  ankerl::unordered_dense::map<EntityID, SystemSignature> signature_map_;
  ComponentManager* component_manager_;
  EntityManager* entity_manager_;
};

}// namespace evie

#endif