#ifndef INCLUDE_ECS_COMPONENT_ARRAY_H_
#define INCLUDE_ECS_COMPONENT_ARRAY_H_

#include <array>
#include <stack>
#include <vector>

#include "ecs_constants.hpp"
#include "evie/ids.h"

namespace evie {

template<typename T> class ComponentWrapper
{
public:
  ComponentWrapper(T component, EntityID id) : component(component), id(id) {}// NOLINT(readability-*)
  T component;
  EntityID id;
};

class IComponentArray
{
public:
  IComponentArray() = default;
  IComponentArray(const IComponentArray&) = delete;
  IComponentArray(IComponentArray&&) = delete;
  IComponentArray& operator=(const IComponentArray&) = delete;
  IComponentArray& operator=(IComponentArray&&) = delete;
  virtual void RemoveComponent(EntityID) = 0;
  virtual ~IComponentArray() = default;
};

template<typename T> class ComponentArray : public IComponentArray
{
public:
  explicit ComponentArray(ComponentID<T> id) : id_(id)// NOLINT(readability-*)
  {
    // First slot in components_ is reserved
    components_.emplace_back(T{}, EntityID(0));
  }
  ComponentArray(ComponentArray&&) noexcept = default;
  ComponentArray(const ComponentArray&) = default;
  ComponentArray& operator=(ComponentArray&&) noexcept = default;
  ComponentArray& operator=(const ComponentArray&) = default;

  void AddComponent(EntityID entity_id, const T& component)
  {
    // Check if there are any free slots in the components_ vector we can use
    // first.
    if (!free_slots_.empty()) {
      // There is a free slot, let's use it instead of allocating more space
      // into the components_ vector.
      const auto& index = free_slots_.top();
      components_[index].component = component;
      components_[index].id = entity_id;
      // Update the entity position map before we overwrite the component
      entity_index_map_[entity_id.Get()] = index;// NOLINT(*-array-index)
      // Pop now we're not using the references from the front of the queue
      free_slots_.pop();
    } else {
      components_.emplace_back(component, entity_id);
      entity_index_map_[entity_id.Get()] = components_.size() - 1;// NOLINT(*-array-index)
    }
  }

  void RemoveComponent(EntityID entity_id) override
  {
    // Check if entity was even added to this component
    // NOLINTNEXTLINE(*-array-index)
    if (entity_index_map_[entity_id.Get()] != 0 && components_.size() - free_slots_.size() != 0) {
      auto& back_component = components_[components_.size() - 1 - free_slots_.size()];
      // Replace removed component with the back of the vector
      components_[entity_index_map_[entity_id.Get()]] = back_component;// NOLINT(*-array-index)
      // Push a free slot onto the queue
      auto back_component_index = entity_index_map_[back_component.id.Get()];
      free_slots_.push(back_component_index);
      // Update the index map with the new position
      entity_index_map_[back_component.id.Get()] = entity_index_map_[entity_id.Get()];// NOLINT(*-array-index)
      // Set the entity_index_map slot to 0 to prove the entity doesn't exist .
      entity_index_map_[entity_id.Get()] = 0;// NOLINT(*-array-index)
    }
  }

  [[nodiscard]] bool HasComponent(EntityID entity_id) const
  {
    return entity_index_map_[entity_id.Get()] != 0;// NOLINT (*-array-index)
  }

  [[nodiscard]] size_t Size() const { return components_.size() - 1 - free_slots_.size(); }

  T& GetComponent(EntityID entity_id)
  {
    return components_[entity_index_map_[entity_id.Get()]].component;// NOLINT (*-array-index)
  }

  std::vector<ComponentWrapper<T>>& GetComponentVector() { return components_; }

  ~ComponentArray() override = default;

private:
  std::array<size_t, MAX_ENTITY_COUNT + 1> entity_index_map_{};
  std::vector<ComponentWrapper<T>> components_;

  // Track free slots in the components_ vector.
  std::stack<size_t> free_slots_;

  // ComponentID that this array represents
  ComponentID<T> id_;
};
}// namespace evie

#endif