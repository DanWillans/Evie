#ifndef INCLUDE_DANDAN_PROJECTILE_SYSTEM_HPP_
#define INCLUDE_DANDAN_PROJECTILE_SYSTEM_HPP_

#include "components.hpp"
#include <evie/default_models.h>
#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/system.hpp>
#include <evie/error.h>
#include <evie/events.h>
#include <evie/ids.h>
#include <evie/input.h>
#include <evie/input_manager.h>
#include <evie/shader_program.h>
#include <evie/texture.h>
#include <evie/types.h>
#include <evie/vertex_buffer.h>

// #include <glm/matrix.hpp>
// #include <glm/ext/quaternion_common.hpp>
#include <glm/ext.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>


class ProjectileSystem : public evie::System
{
public:
  ProjectileSystem(evie::ECSController* ecs,
    evie::ComponentID<evie::MeshComponent> mesh_cid,
    evie::ComponentID<evie::TransformComponent> transform_cid,
    evie::ComponentID<ProjectileComponent> projectile_cid,
    evie::ComponentID<VelocityComponent> velocity_cid,
    evie::Entity* player_entity,
    const float& map_boundary)
    : ecs_(ecs), mesh_cid_(mesh_cid), transform_cid_(transform_cid), projectile_cid_(projectile_cid),
      velocity_cid_(velocity_cid), player_entity_(player_entity)
  {
    constexpr float half_map_size = 2.0F;
    map_boundary_ = map_boundary / half_map_size;
  }

  ProjectileSystem(const ProjectileSystem&) = delete;
  ProjectileSystem(ProjectileSystem&&) = delete;
  ProjectileSystem& operator=(const ProjectileSystem&) = delete;
  ProjectileSystem& operator=(ProjectileSystem&&) = delete;
  ~ProjectileSystem() override = default;

  static constexpr float ProjectileSpeed = 40.0F;

  evie::Error Initialise()
  {
    evie::Error err = evie::Error::OK();
    if (err.Good()) {
      err = vs_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
    }
    if (err.Good()) {
      err = fs_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
    }
    // Floor texture
    if (err.Good()) {
      err = tex_.Initialise(
        R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\grass.jpg)");
    }
    if (err.Good()) {
      err = shader_program_.Initialise(&vs_, &fs_);
    }
    return err;
  }

  evie::Error MousePressed()
  {
    APP_INFO("Mouse pressed");
    auto err = CreateProjectile();
    if (err.Bad()) {
      APP_INFO("Error creating projectile: %s", err.Message());
    }
    return err;
  }

private:
  void Update(const float& delta_time) override
  {
    (void)delta_time;

    for (const auto& entity : entities) {
      const auto& transform = entity.GetComponent(transform_cid_);

      // Delete projectiles if they go out of bounds.
      // NOLINTNEXTLINE(*-union-access)
      if (fabs(transform.position.x) > map_boundary_ || fabs(transform.position.y) > map_boundary_
          || fabs(transform.position.z) > map_boundary_) {// NOLINT(*-union-access)
        APP_INFO("Delete entity {}", entity.GetID().Get());
        MarkEntityForDeletion(entity);
      }
    }
  }

  evie::Error CreateProjectile()
  {
    evie::Error err = evie::Error::OK();
    constexpr evie::vec3 projectile_scale{ 0.2F, 0.2F, 0.2F };
    constexpr evie::vec3 projectile_local_offset{ 0.1F, -0.2F, -0.4F };
    auto entity = ecs_->CreateEntity();
    APP_INFO("Projectile ID: {}", entity->GetID().Get());

    if (entity) {
      // Create the transform for the projectile.
      // Really it'd be good to set a parent transform here(the player entity) then we could have a static transform
      // offset. We don't support this right now so we'll have to move the block with the player/camera.
      const auto& player_transform = player_entity_->GetComponent(transform_cid_);
      evie::TransformComponent transform;
      transform.position = player_transform.position + player_transform.rotation * projectile_local_offset;
      transform.scale = projectile_scale;
      transform.rotation = player_transform.rotation;
      err = entity->AddComponent(transform_cid_, transform);
      if (err.Good()) {
        evie::MeshComponent mesh_component;
        evie::BufferLayout layout;
        layout.stride = 5;// NOLINT
        layout.type = evie::VertexDataType::Float;
        layout.layout_sizes = { 3, 2 };
        err = mesh_component.model_data.Initialise(evie::default_models::cube, layout);
        if (err.Good()) {
          mesh_component.vertex_array.Initialise();
          err = mesh_component.vertex_array.AssociateVertexBuffer(mesh_component.model_data);
        }
        mesh_component.shader_program = shader_program_;
        if (err.Good()) {
          mesh_component.shader_program.Use();
          mesh_component.shader_program.SetInt("Texture1", 0);
          mesh_component.texture = tex_;
          err = entity->AddComponent(mesh_cid_, mesh_component);
        }
        if (err.Good()) {
          err = entity->AddComponent(projectile_cid_);
        }
        if (err.Good()) {
          err = entity->AddComponent(
            velocity_cid_, VelocityComponent{ .velocity = evie::vec3{ 0.0F, 0.0F, -ProjectileSpeed } });
          APP_INFO("Made all");
        }
      }
    } else {
      err = entity.Error();
    }
    return err;
  }
  evie::ECSController* ecs_{ nullptr };
  evie::ComponentID<evie::MeshComponent> mesh_cid_{ 0 };
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };
  evie::ComponentID<ProjectileComponent> projectile_cid_{ 0 };
  evie::ComponentID<VelocityComponent> velocity_cid_{ 0 };
  evie::Entity* player_entity_{ nullptr };
  evie::VertexShader vs_;
  evie::FragmentShader fs_;
  evie::Texture2D tex_;
  evie::ShaderProgram shader_program_;
  float map_boundary_;
};

#endif// !INCLUDE_DANDAN_PROJECTILE_SYSTEM_HPP_