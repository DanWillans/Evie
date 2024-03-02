#ifndef EVIE_ECS_INCLUDE_COMPONENT_ARRAY_H_
#define EVIE_ECS_INCLUDE_COMPONENT_ARRAY_H_

#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#include "ecs_constants.hpp"
#include "evie/core.h"
#include "evie/error.h"
#include "evie/ids.h"
#include "evie/logging.h"
#include "evie/result.h"
#include "evie/types.h"


namespace evie {

template<typename T> struct ComponentWrapper
{
  ComponentWrapper(T component, EntityID entity_id) : component(component), entity_id(entity_id) {}
  bool dirty{ false };
  T component;
  EntityID entity_id;
};

// Convenience class for iterating over only valid components of a ComponentArray
template<typename T> class EVIE_API ComponentIterator
{
public:
  explicit ComponentIterator(std::vector<ComponentWrapper<T>>& component_array) : component_array_{ component_array }
  {
    // Check we're at a valid index first
    LoopToNextValidIndex();
  }

  ComponentIterator(const ComponentIterator<T>& other) : component_array_(other.component_array_), index_(other.index_)
  {}

  ComponentIterator<T>& operator=(const ComponentIterator<T>& other)
  {
    component_array_ = other.component_array_;
    index_ = other.index_;
    return *this;
  }

  [[nodiscard]] bool Valid() const
  {
    // Any data in our component_array?
    if (component_array_.empty()) {
      return false;
    }

    // Are we out of bounds?
    if (index_ > component_array_.size() - 1) {
      return false;
    }
    return true;
  }

  // Must only be called after checking Valid() returns true.
  T* operator->() { return &component_array_[index_].component; }

  void Next()
  {
    index_++;
    LoopToNextValidIndex();
  }

private:
  void LoopToNextValidIndex()
  {
    do {
      if (Valid()) {
        // Check if the component we're pointing at is dirty or not
        if (component_array_[index_].dirty) {
          index_++;
        } else {
          break;
        }
      }
    } while (Valid());
  }
  std::vector<ComponentWrapper<T>>& component_array_;
  size_t index_{ 0 };
};

class IComponentArray
{
public:
  virtual void RemoveComponent(EntityID) = 0;
  virtual ~IComponentArray() = default;
};

template<typename T> class ComponentArray : public IComponentArray
{
public:
  explicit ComponentArray(ComponentID<T> id) : id_(id) {}
  ComponentArray(ComponentArray&&) = default;
  ComponentArray(const ComponentArray&) = default;
  ComponentArray& operator=(ComponentArray&&) = default;
  ComponentArray& operator=(const ComponentArray&) = default;
  void AddComponent(EntityID entity_id, const T& component)
  {
    // Check if there are any free slots in the components_ vector we can use first.
    if (!free_slots_.empty()) {
      // There is a free slot, let's use it instead of allocating more space into the components_ vector.
      const auto& index = free_slots_.front();
      auto& array_component = components_[index];
      // Update the entity position map before we overwrite the component
      entity_index_map_[entity_id] = index;
      // Update the component
      array_component.component = component;
      array_component.entity_id = entity_id;
      array_component.dirty = false;
      // Pop now we're not using the references from the front of the queue
      free_slots_.pop();
    } else {
      components_.emplace_back(component, entity_id);
      entity_index_map_[entity_id] = components_.size() - 1;
    }
  }

  void RemoveComponent(EntityID entity_id) override
  {
    auto entity_it = entity_index_map_.find(entity_id);
    if (entity_it != entity_index_map_.end()) {
      // Invalidate the component in the vector.
      components_[entity_it->second].dirty = true;
      free_slots_.push(entity_it->second);
      // Remove the component from the entity map.
      entity_index_map_.erase(entity_id);
    } else {
      EV_WARN("EntityID: {} - doesn't exist for component id: {}", entity_id.Get(), id_.Get());
    }
  }

  [[nodiscard]] size_t Size() const { return components_.size() - free_slots_.size(); }

  ComponentIterator<T> GetComponentIterator() { return ComponentIterator<T>(components_); }

  T& GetComponent(EntityID entity_id) { return components_[entity_index_map_[entity_id]].component; }

  ~ComponentArray<T>() override = default;

private:
  // This unordered_map will map entities to component indexes in the components_ vector.
  // This map should only be used for inserting and deleting entities from components.
  std::unordered_map<EntityID, size_t> entity_index_map_;

  // This vector will hold our component entries. Not all components entries are valid.
  // When iterating over the components it is necessary to check the dirty flag in the ComponentWrapper.
  std::vector<ComponentWrapper<T>> components_;

  // Track free slots in the components_ vector.
  std::queue<size_t> free_slots_;

  // ComponentID that this array represents
  ComponentID<T> id_;
};

}// namespace evie

#endif// EVIE_ECS_INCLUDE_COMPONENT_ARRAY_H_