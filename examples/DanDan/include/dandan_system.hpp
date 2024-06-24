#ifndef INCLUDE_DANDAN_SYSTEM_HPP_
#define INCLUDE_DANDAN_SYSTEM_HPP_

#include <evie/ecs/system_signature.hpp>
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
    evie::ECSController* ecs)
    : enemy_cid_(enemy_cid), mesh_cid_(mesh_cid), transform_cid_(transform_cid), follower_cid_(follower_cid),
      projectile_cid_(projectile_cid), ecs_(ecs)
  {}

  static constexpr evie::vec3 dandan_scale{ 2.0F, 2.0F, 2.0F };
  static constexpr float dandan_height_offset{ 1.5F };

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

    // Make the first DanDan
    CreateDanDan();

    evie::SystemSignature signature;
    signature.SetComponent(projectile_cid_);
    projectiles_ = RegisterSystemSignature(signature);

    if (!projectiles_) {
      err = evie::Error{ "Failed to get projectiles vector" };
    }

    return err;
  }

private:
  void Update(const float& delta_time)
  {
    (void)delta_time;
    if (entities.empty()) {
      CreateDanDan();
    }
    // Iterate over DanDans and check if any projectile entities have hit it
    for (const auto& dandan : GetEntities()) {
      // Iterate all projectiles and check if they intersect with DanDan
      for (const auto& projectile : *projectiles_) {
        // Check projectile has transform component first
        // if (component_manager->HasComponent(projectile.id, transform_cid_)) {
        const auto& transform = projectile.GetComponent(transform_cid_);
        const auto& dandan_transform = dandan.GetComponent(transform_cid_);
        if (Collides(transform, dandan_transform)) {
          // Delete dandan
          MarkEntityForDeletion(dandan);
          // Delete projectile
          MarkEntityForDeletion(projectile);
          continue;
        }
      }
    }
  }

  bool Collides(const evie::TransformComponent& transform, const evie::TransformComponent& dandan_transform)
  {
    auto diff_x = fabs(transform.position.x - dandan_transform.position.x);
    auto diff_y = fabs(transform.position.y - dandan_transform.position.y);
    auto diff_z = fabs(transform.position.z - dandan_transform.position.z);
    return diff_x < dandan_scale.x / 2.0F && diff_y < dandan_scale.y / 2.0F && diff_z < dandan_scale.z / 2.0F;
  }

  evie::Error CreateDanDan()
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
    APP_INFO("dandan id: {}", dandan->GetID().Get());
    if (dandan && err.Good()) {
      err = dandan->AddComponent(mesh_cid_, mesh_component);
      if (err.Good()) {
        evie::TransformComponent transform;
        // NOLINTNEXTLINE(*-union-access)
        transform.position.y = dandan_height_offset;
        transform.scale = dandan_scale;
        err = dandan->AddComponent(transform_cid_, transform);
        if (err.Good()) {
          // Dan dan is a follower
          err = dandan->AddComponent(follower_cid_);
        }
        if (err.Good()) {
          err = dandan->AddComponent(enemy_cid_);
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
  evie::ECSController* ecs_{ nullptr };
  ankerl::unordered_dense::set<evie::Entity>* projectiles_{ nullptr };
};

#endif// !INCLUDE_DANDAN_SYSTEM_HPP_