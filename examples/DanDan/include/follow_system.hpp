#ifndef INCLUDE_DANDAN_FOLLOW_SYSTEM_HPP_
#define INCLUDE_DANDAN_FOLLOW_SYSTEM_HPP_

#include "components.hpp"
#include "evie/ids.h"

#include "evie/ecs/system.hpp"
#include <evie/ecs/component_array.hpp>
#include <evie/ecs/components/Transform.hpp>
#include <evie/logging.h>
#include <glm/geometric.hpp>

class FollowSystem : public evie::System
{
public:
  FollowSystem(evie::ComponentID<FollowTargetComponent> follow_target_id,
    evie::ComponentID<FollowerComponent> follower_id_,
    evie::ComponentID<evie::TransformComponent> transform_cid)
    : follower_cid_(follower_id_), follow_target_cid_(follow_target_id), transform_cid_(transform_cid)
  {}
  FollowSystem(const FollowSystem&) = delete;
  FollowSystem(FollowSystem&&) = delete;
  FollowSystem& operator=(const FollowSystem&) = delete;
  FollowSystem& operator=(FollowSystem&&) = delete;
  virtual ~FollowSystem() = default;

  void Initialise() { follow_target_vector_ = &GetComponentVector(follow_target_cid_); }

private:
  void Update(const float& delta_time) override
  {
    // Should only ever be one target for now. Which means vector size 2 :| :| :\. Fix this.
    if ((follow_target_vector_ != nullptr) && follow_target_vector_->size() == 2) {
      // Get the target EntityID
      const evie::EntityID& entity_id = (*follow_target_vector_)[1].id;
      // Check if the target entity has a transform component.
      if (component_manager->HasComponent(entity_id, transform_cid_)) {
        const auto& target_transform = component_manager->GetComponent(entity_id, transform_cid_);
        // Iterate all of the Followers and move them towards the target
        for (const auto& entity : entities) {
          auto& transform = entity.GetComponent(transform_cid_);
          // Calculate distance travelled by speed * time. In future should add a velocity component.
          const float distance = 5.0F * delta_time;
          // Calculate the direction vector. Target - Follower
          auto diff = target_transform.position - transform.position;
          // Normalise and multiply by distance.
          auto norm = glm::normalize(diff) * distance;
          //NOLINTNEXTLINE(*-union-access)
          transform.position += evie::vec3{ norm.x, 0.0, norm.z };
        }
      }
    } else {
      APP_INFO("No target to follow");
    }
  }

  evie::ComponentID<FollowerComponent> follower_cid_{ 0 };
  evie::ComponentID<FollowTargetComponent> follow_target_cid_{ 0 };
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };
  std::vector<evie::ComponentWrapper<FollowTargetComponent>>* follow_target_vector_{ nullptr };
};

#endif// !INCLUDE_DANDAN_FOLLOW_SYSTEM_HPP_