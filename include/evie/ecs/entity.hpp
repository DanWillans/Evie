#ifndef INCLUDE_ECS_ENTITY_HPP_
#define INCLUDE_ECS_ENTITY_HPP_

#include "component_array.hpp"
#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager_interface.hpp"

#include "ankerl/unordered_dense.h"

namespace evie {

// A convenience class for handling an Entity. It shouldn't hold any state other than an EntityID
// We want to be able to add and remove Components from Entities
class Entity
{
public:
  [[nodiscard]] EntityID GetID() const { return id_; }

  template<typename ComponentName>
  [[nodiscard]] Error AddComponent(ComponentID<ComponentName> component_id,
    const ComponentName& component = ComponentName{})
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

  template<typename ComponentName>
  [[nodiscard]] ComponentName& GetComponent(ComponentID<ComponentName> component_id) const
  {
    return component_manager_->GetComponent(id_, component_id);
  }

  void Destroy() const
  {
    component_manager_->EntityDestroyed(id_);
    system_manager_->EntityDestroyed(id_);
    entity_manager_->DestroyEntity(id_);
  }

  // I don't like this. Fix it in the future.
  void MoveEntity(Entity*& other)
  {
    void* mem = malloc(sizeof(Entity)); //NOLINT
    memcpy(mem, this, sizeof(Entity));
    other = static_cast<Entity*>(mem);
  }

  bool operator==(const Entity& rhs) const { return rhs.id_ == this->id_; }

  [[nodiscard]] bool IsValid() const { return valid_; }

private:
  friend class ECSController;
  friend class SystemManager;
  Entity(ISystemManager* system_manager,
    ComponentManager* component_manager,
    EntityManager* entity_manager,
    EntityID entity_id)
    : id_(entity_id), system_manager_(system_manager), component_manager_(component_manager),
      entity_manager_(entity_manager), valid_(true)
  {}
  // Default constructor, creates an INVALID entity.
  Entity() : id_(0) {}
  EntityID id_;
  ISystemManager* system_manager_{ nullptr };
  ComponentManager* component_manager_{ nullptr };
  EntityManager* entity_manager_{ nullptr };
  bool valid_{ false };
};

}// namespace evie

namespace std {
template<> struct hash<evie::Entity>
{
  uint64_t operator()(const evie::Entity& identifier) const noexcept { return identifier.GetID().Get(); }
};
}// namespace std

template<> struct ankerl::unordered_dense::hash<evie::Entity>
{
  using is_avalanching = void;

  [[nodiscard]] auto operator()(evie::Entity const& x) const noexcept -> uint64_t
  {
    return detail::wyhash::hash(x.GetID().Get());
  }
};
#endif// !INCLUDE_ECS_ENTITY_HPP_