#ifndef INCLUDE_COMPONENT_MANAGER_H_
#define INCLUDE_COMPONENT_MANAGER_H_

#include <array>
#include <cstddef>
#include <memory>

#include "component_array.hpp"
#include "ecs_constants.hpp"
#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"

namespace evie {

class EVIE_API ComponentManager
{
public:
  ComponentManager() = default;
  ComponentManager(const ComponentManager&) = delete;
  ComponentManager& operator=(const ComponentManager&) = delete;

  // Chose to use ComponentID so that we don't have to do any fancy
  // type resolution with typeid to get the correct IComponentArray.
  template<typename ComponentName> ComponentID<ComponentName> RegisterComponent()
  {
    ComponentID<ComponentName> comp_id(component_index_count_++);
    components_[comp_id.Get()] = std::make_unique<ComponentArray<ComponentName>>(comp_id);
    return comp_id;
  }

  template<typename ComponentName>
  Error AddComponent(EntityID entity_id, ComponentID<ComponentName> component_id, const ComponentName& comp) const
  {
    if (component_id.Get() >= component_index_count_) {
      return Error{ "Component Index out of bounds" };
    }
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    comp_array->AddComponent(entity_id, comp);
    return Error::OK();
  }

  template<typename ComponentName> Error RemoveComponent(EntityID entity_id, ComponentID<ComponentName> component_id)
  {
    if (component_id.Get() >= component_index_count_) {
      return Error{ "Component Index out of bounds" };
    }
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    comp_array->RemoveComponent(entity_id);
    return Error::OK();
  }

  void EntityDestroyed(EntityID entity_id);

  template<typename ComponentName>
  ComponentName& GetComponent(EntityID entity_id, ComponentID<ComponentName> component_id)
  {
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    return comp_array->GetComponent(entity_id);
  }

  template<typename ComponentName> size_t GetComponentCount(ComponentID<ComponentName> component_id)
  {
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    return comp_array->Size();
  }

  template<typename ComponentName>
  std::vector<ComponentWrapper<ComponentName>>& GetComponentVector(ComponentID<ComponentName> component_id)
  {
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    return comp_array->GetComponentVector();
  }

  template<typename ComponentName>
  bool HasComponent(EntityID id, ComponentID<ComponentName> component_id)
  {
    auto* comp_array = static_cast<ComponentArray<ComponentName>*>(components_[component_id.Get()].get());
    return comp_array->HasComponent(id);
  }



private:
  // The index for a specific component in this array maps to it's ComponentID
  // value
  std::array<std::unique_ptr<IComponentArray>, MAX_COMPONENT_COUNT> components_;
  // A count to store how "full" or components_ array is.
  size_t component_index_count_{ 0 };
};
}// namespace evie

#endif