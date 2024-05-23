#ifndef INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_
#define INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_

#include "evie/ids.h"
#include "system_signature.hpp"


namespace evie {
class Entity;

class ISystemManager
{
public:
  virtual void EntityDestroyed(EntityID entity) = 0;
  virtual void EntitySignatureChanged(EntityID entity_id, const SystemSignature& new_entity_signature) = 0;
  [[nodiscard]] virtual SystemSignature& GetEntitySystemSignature(EntityID entity_id) = 0;
};
}// namespace evie

#endif// !INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_