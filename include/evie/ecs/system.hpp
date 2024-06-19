#ifndef INCLUDE_ECS_SYSTEM_HPP_
#define INCLUDE_ECS_SYSTEM_HPP_

#include <unordered_map>

#include "component_manager.hpp"
#include "entity.hpp"
#include "evie/ids.h"
#include "system_signature.hpp"

#include "ankerl/unordered_dense.h"

namespace evie {
// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// It also includes some helper functions to the System.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
// It is recommended to use an Initialise() function instead of doing logic in the constructor. This is because
// additional configuration to the system is done after construction.
// NOLINTNEXTLINE
class EVIE_API System
{
public:
  virtual ~System() = default;

  // A set of entities that this system is interested in based on the SystemSignature
  ankerl::unordered_dense::set<Entity> entities;
  // The signature of components this system cares about.
  SystemSignature signature;
  // A handle to the component manager
  // Do not use until the system has been registered.
  ComponentManager* component_manager{ nullptr };

  /**
   * @brief This should be called by the game. Internally it will call the user implemented Update() function.
   *
   * @param delta_time The time between previous frame and current.
   */
  void UpdateSystem(const float& delta_time);

  // Get a specific ComponentID vector.
  // You may want to access entities that don't reflect your system signature.
  // You must not use this function until after the system has been Registered with the system manager.
  template<typename ComponentName>
  std::vector<ComponentWrapper<ComponentName>>& GetComponentVector(ComponentID<ComponentName> identifier)
  {
    return component_manager->GetComponentVector(identifier);
  }

protected:
  void MarkEntityForDeletion(const Entity& entity);

private:
  /**
   * @brief This is for the developer to implement. This is where entities should be marked as deleted if they should be
   * removed.
   *
   * @param delta_time The time between previous frame and current.
   */
  virtual void Update([[maybe_unused]] const float& delta_time) = 0;

  // We don't expose std::vector in the API so just disable the warning here.
  #pragma warning( disable: 4251 )
  ankerl::unordered_dense::set<Entity> entities_to_delete_;
};
}// namespace evie

#endif// !INCLUDE_ECS_SYSTEM_HPP_