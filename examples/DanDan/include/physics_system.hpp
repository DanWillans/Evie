#ifndef INCLUDE_DANDAN_PHYSICS_SYSTEM_HPP_
#define INCLUDE_DANDAN_PHYSICS_SYSTEM_HPP_

#include "components.hpp"
#include <evie/ecs/system.hpp>
#include <evie/ecs/components/transform.hpp>
#include <evie/ids.h>

class PhysicsSystem : public evie::System
{
public:
  PhysicsSystem(evie::ComponentID<VelocityComponent> velocity_cid,
    evie::ComponentID<evie::TransformComponent> transform_cid)
    : velocity_cid_(velocity_cid), transform_cid_(transform_cid)
  {}
  PhysicsSystem(const PhysicsSystem&) = delete;
  PhysicsSystem(PhysicsSystem&&) = delete;
  PhysicsSystem& operator=(const PhysicsSystem&) = delete;
  PhysicsSystem& operator=(PhysicsSystem&&) = delete;
   ~PhysicsSystem() override = default;

private:
  void Update(const float& delta_time) override
  {
    for (const auto& entity : entities) {
      const auto& velocity = entity.GetComponent(velocity_cid_);
      auto& transform = entity.GetComponent(transform_cid_);
      // velocity is local velocity
      // Calculate the distance travelled in local co-ordinate system
      auto dist_x = delta_time * velocity.velocity.x;// NOLINT(*-union-access)
      auto dist_y = delta_time * velocity.velocity.y;// NOLINT(*-union-access)
      auto dist_z = delta_time * velocity.velocity.z;// NOLINT(*-union-access)
      // rotate the local velocity to global position.
      transform.position += transform.rotation * evie::vec3{ dist_x, dist_y, dist_z };
    }
  }
  evie::ComponentID<VelocityComponent> velocity_cid_{ 0 };
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };
};

#endif// INCLUDE_DANDAN_PHYSICS_SYSTEM_HPP_