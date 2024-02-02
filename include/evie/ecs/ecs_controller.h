#ifndef EVIE_ECS_INCLUDE_ECS_CONTROLLER_H_
#define EVIE_ECS_INCLUDE_ECS_CONTROLLER_H_

#include "evie/ecs/component_manager.hpp"
#include "evie/ecs/entity_manager.hpp"
#include "evie/ecs/system_manager.hpp"
#include "evie/error.h"
#include "evie/ids.h"

#include <memory>

namespace evie {

// A convenience class for handling an Entity. It shouldn't hold any state other than an EntityID
// We want to be able to add and remove Components from Entities
class Entity
{
public:
  [[nodiscard]] EntityID GetID() const { return id_; }

  template<typename ComponentName>
  [[nodiscard]] Error AddComponent(ComponentID<ComponentName> component_id, const ComponentName& component)
  {
    // Add this entities component to the component manager
    auto err = component_manager_->AddComponent(id_, component_id, component);
    if (err.Good()) {
      // Update the system manager with the entities new system signature.
      SystemSignature& current_signature = system_manager_->GetEntitySystemSignature(id_);
      current_signature.SetComponent(component_id);
      system_manager_->EntitySignatureChanged(id_, current_signature);
    }
    return err;
  }

  template<typename ComponentName> [[nodiscard]] Error RemoveComponent(ComponentID<ComponentName> component_id)
  {
    // Remove this entities component from the component manager
    auto err = component_manager_->RemoveComponent(id_, component_id);
    if (err.Good()) {
      // Update the system manager with the entities new system signature
      SystemSignature current_signature = system_manager_->GetEntitySystemSignature(id_);
      current_signature.ResetComponent(component_id);
      system_manager_->EntitySignatureChanged(id_, current_signature);
    }
    return err;
  }

  void Destroy()
  {
    component_manager_->EntityDestroyed(id_);
    system_manager_->EntityDestroyed(id_);
    entity_manager_->DestroyEntity(id_);
  }

  bool operator==(const Entity& rhs) const { return rhs.id_ == this->id_; }

private:
  friend class ECSController;
  Entity(SystemManager* system_manager,
    ComponentManager* component_manager,
    EntityManager* entity_manager,
    EntityID entity_id)
    : id_(entity_id), system_manager_(system_manager), component_manager_(component_manager),
      entity_manager_(entity_manager)
  {}
  EntityID id_;
  SystemManager* system_manager_;
  ComponentManager* component_manager_;
  EntityManager* entity_manager_;
};

class ECSController
{
public:
  ECSController()
    : component_manager_(std::make_unique<ComponentManager>()),
      system_manager_(std::make_unique<SystemManager>(component_manager_.get())),
      entity_manager_(std::make_unique<EntityManager>())
  {}

  Result<Entity> CreateEntity()
  {
    Result<EntityID> entity_id = entity_manager_->CreateEntity();
    if (entity_id.Bad()) {
      return entity_id.Error();
    }
    return Entity{ system_manager_.get(), component_manager_.get(), entity_manager_.get(), *entity_id };
  }

  template<typename ComponentName> [[nodiscard]] ComponentID<ComponentName> RegisterComponent()
  {
    return component_manager_->RegisterComponent<ComponentName>();
  }

  template<typename SystemName, typename... Args>
  [[nodiscard]] SystemID<SystemName> RegisterSystem(const SystemSignature& signature, Args... args)
  {
    return system_manager_->RegisterSystem<SystemName>(signature, args...);
  }

  template<typename SystemName> const SystemName* GetSystem(SystemID<SystemName> system_id)
  {
    return static_cast<const SystemName*>(&system_manager_->GetSystem(system_id));
  }

  [[nodiscard]] uint64_t EntityCount() const { return entity_manager_->EntityCount(); }

private:
  // Create these all on the heap because they could be quite large
  std::unique_ptr<ComponentManager> component_manager_;
  std::unique_ptr<SystemManager> system_manager_;
  std::unique_ptr<EntityManager> entity_manager_;
};

}// namespace evie

#endif// !EVIE_ECS_INCLUDE_ECS_CONTROLLER_H_