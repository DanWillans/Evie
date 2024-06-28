#ifndef INCLUDE_DANDAN_SYSTEM_HPP_
#define INCLUDE_DANDAN_SYSTEM_HPP_

#include <cstdlib>
#include <evie/ecs/components/velocity.hpp>
#include <evie/ecs/entity.hpp>
#include <evie/ecs/system_signature.hpp>
#include <optional>
#include <vector>

#include "components.hpp"

#include <evie/default_models.h>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/components/transform.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/system.hpp>
#include <evie/ids.h>
#include <evie/shader.h>
#include <evie/shader_program.h>
#include <evie/texture.h>
#include <evie/types.h>
#include <evie/vertex_array.h>

#include <glm/ext/quaternion_trigonometric.hpp>


class DanDanSystem : public evie::System
{
public:
  DanDanSystem(evie::ComponentID<EnemyComponent> enemy_cid,
    evie::ComponentID<evie::MeshComponent> mesh_cid,
    evie::ComponentID<evie::TransformComponent> transform_cid,
    evie::ComponentID<FollowerComponent> follower_cid,
    evie::ComponentID<ProjectileComponent> projectile_cid,
    evie::ComponentID<FollowTargetComponent> target_cid,
    evie::ComponentID<VelocityComponent> velocity_cid,
    evie::ECSController* ecs,
    float map_scale)
    : enemy_cid_(enemy_cid), mesh_cid_(mesh_cid), transform_cid_(transform_cid), follower_cid_(follower_cid),
      projectile_cid_(projectile_cid), target_cid_(target_cid), velocity_cid_(velocity_cid), ecs_(ecs),
      map_scale_(map_scale)
  {}

  static constexpr evie::vec3 dandan_scale{ 2.0F, 2.0F, 2.0F };
  static constexpr evie::vec3 starting_offset{ 0.0F, 1.5F, -10.0F };

  static constexpr int score_to_win{ 15 };

  evie::Error Initialise()
  {
    evie::Error err = evie::Error::OK();
    if (err.Good()) {
      err = vs_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
    }
    if (err.Good()) {
      err = fs_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
    }
    // DanDan texture
    if (err.Good()) {
      err = tex_.Initialise(
        R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\dandan.png)", true);
    }
    if (err.Good()) {
      err = shader_program_.Initialise(&vs_, &fs_);
    }

    evie::SystemSignature proj_signature;
    proj_signature.SetComponent(projectile_cid_);
    evie::Result<evie::EntitySet*> proj_sys = RegisterSystemSignature(proj_signature);
    if (proj_sys.Bad()) {
      err = evie::Error{ "Failed to get projectiles entity set" };
    }
    projectiles_ = *proj_sys;

    evie::SystemSignature target_signature;
    target_signature.SetComponent(target_cid_);
    evie::Result<evie::EntitySet*> target_sys = RegisterSystemSignature(target_signature);
    if (target_sys.Bad()) {
      err = evie::Error{ "Failed to get entity set" };
    }
    targets_ = *target_sys;

    return err;
  }

private:
  void Update(const float& delta_time) override
  {
    (void)delta_time;
    if (GetEntities().empty()) {
      evie::TransformComponent transform;
      transform.position = starting_offset;
      transform.scale = dandan_scale;
      CreateDanDan(transform, true, static_cast<float>(score_));
    }
    // Iterate over DanDans and check if any projectile entities have hit it
    for (const auto& dandan : GetEntities()) {
      // Iterate all projectiles and check if they intersect with DanDan
      auto& dandan_transform = dandan.GetComponent(transform_cid_);
      for (const auto& projectile : *projectiles_) {
        const auto& transform = projectile.GetComponent(transform_cid_);
        if (Collides(transform, dandan_transform)) {
          APP_INFO("Mark entity id {} for deletion", dandan.GetID().Get());
          // Delete dandan
          MarkEntityForDeletion(dandan);
          // Delete projectile
          MarkEntityForDeletion(projectile);
          // Increment the score as we've killed a DanDan
          score_++;
          // Create a new DanDan as a way to keep score
          evie::TransformComponent score_transform;
          score_transform.position.x = (-map_scale_ / 2.0F) + score_ * 2.0F;// NOLINT
          score_transform.position.y = 5.0F;// NOLINT
          score_transform.position.z = -map_scale_ / 2.0F;// NOLINT
          next_dandan_transform_ = score_transform;
          continue;
        }
      }
      for (const auto& target : *targets_) {
        const auto& transform = target.GetComponent(transform_cid_);
        if (Collides(transform, dandan_transform)) {
          // If already colliding then break so that we don't ruin the score.
          if (colliding_) {
            dandan_transform.position = starting_offset;
            break;
          }
          // Reduce the score
          if (score_ > 0) {
            score_--;
          }
          // Delete the DanDan scorers
          if (!score_dan_dans_.empty()) {
            score_dan_dans_.back().Destroy();
            score_dan_dans_.erase(score_dan_dans_.end() - 1);
          }
          // Reset DanDan position if it catches you
          APP_INFO("Reset dandan");
          dandan_transform.position = starting_offset;
          colliding_ = true;
        } else {
          colliding_ = false;
        }
      }
      if (next_dandan_transform_.has_value()) {
        CreateDanDan(next_dandan_transform_.value(), false);
        next_dandan_transform_ = std::nullopt;
      }
      if (score_ == score_to_win) {
        APP_INFO("You win!");
        std::exit(EXIT_SUCCESS);
      }
    }
  }

  static bool Collides(const evie::TransformComponent& transform, const evie::TransformComponent& dandan_transform)
  {
    auto diff_x = fabs(transform.position.x - dandan_transform.position.x);// NOLINT
    auto diff_y = fabs(transform.position.y - dandan_transform.position.y);// NOLINT
    auto diff_z = fabs(transform.position.z - dandan_transform.position.z);// NOLINT
    return diff_x < dandan_scale.x / 2.0F && diff_y < dandan_scale.y / 2.0F && diff_z < dandan_scale.z / 2.0F;// NOLINT
  }

  evie::Error CreateDanDan(bool enemy = true, float speed = 0.0F)
  {
    evie::TransformComponent transform;
    // NOLINTNEXTLINE(*-union-access)
    transform.position = starting_offset;
    transform.scale = dandan_scale;
    return CreateDanDan(transform, enemy, speed);
  }

  evie::Error CreateDanDan(const evie::TransformComponent& transform, bool enemy = true, float speed = 0.0F)
  {
    evie::Error err = evie::Error::OK();
    evie::MeshComponent mesh_component;
    // We need to create the layout for our model/vertex data
    evie::BufferLayout layout;
    layout.stride = 5;// NOLINT
    layout.type = evie::VertexDataType::Float;
    layout.layout_sizes = { 3, 2 };
    if (err.Good()) {
      // Initialise the vertex buffer
      err = mesh_component.model_data.Initialise(evie::default_models::cube_texture_up_right, layout);
    }
    if (err.Good()) {
      // Associate the vertex buffer with the vertex array
      mesh_component.vertex_array.Initialise();
      err = mesh_component.vertex_array.AssociateVertexBuffer(mesh_component.model_data);
    }
    mesh_component.shader_program = shader_program_;
    mesh_component.shader_program.Use();
    mesh_component.shader_program.SetInt("Texture1", 0);
    mesh_component.texture = tex_;

    auto dandan = ecs_->CreateEntity();
    if (dandan && err.Good()) {
      err = dandan->AddComponent(mesh_cid_, mesh_component);
      if (err.Good()) {
        err = dandan->AddComponent(transform_cid_, transform);
        if (err.Good() && enemy) {
          // Dan dan is a follower
          err = dandan->AddComponent(follower_cid_);
        }
        if (err.Good() && enemy) {
          err = dandan->AddComponent(enemy_cid_);
        }
        if (err.Good() && enemy) {
          VelocityComponent vel_comp;
          vel_comp.speed = speed * 4.0F;
          err = dandan->AddComponent(velocity_cid_, vel_comp);
        }
        if (err.Good() && !enemy) {
          score_dan_dans_.push_back(*dandan);
        }
      }
    }
    return err;
  }

  evie::VertexShader vs_;
  evie::FragmentShader fs_;
  evie::Texture2D tex_;
  evie::ShaderProgram shader_program_;
  evie::ComponentID<EnemyComponent> enemy_cid_{ 0 };
  evie::ComponentID<evie::MeshComponent> mesh_cid_{ 0 };
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };
  evie::ComponentID<FollowerComponent> follower_cid_{ 0 };
  evie::ComponentID<ProjectileComponent> projectile_cid_{ 0 };
  evie::ComponentID<FollowTargetComponent> target_cid_{ 0 };
  evie::ComponentID<VelocityComponent> velocity_cid_{ 0 };
  evie::ECSController* ecs_{ nullptr };
  evie::EntitySet* projectiles_{ nullptr };
  evie::EntitySet* targets_{ nullptr };
  std::vector<evie::Entity> score_dan_dans_;
  int score_{ 0 };
  float map_scale_{ 0.0 };
  std::optional<evie::TransformComponent> next_dandan_transform_{ std::nullopt };
  bool colliding_{ false };
};

#endif// !INCLUDE_DANDAN_SYSTEM_HPP_