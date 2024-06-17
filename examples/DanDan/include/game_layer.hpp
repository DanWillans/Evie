#ifndef INCLUDE_GAME_LAYER_HPP_
#define INCLUDE_GAME_LAYER_HPP_

#include "components.hpp"
#include "follow_system.hpp"
#include "physics_system.hpp"
#include "projectile_system.hpp"
#include "dandan_system.hpp"
#include "render.hpp"

#include <evie/camera.h>
#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/components/velocity.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/entity.hpp>
#include <evie/error.h>
#include <evie/ids.h>
#include <evie/input_manager.h>
#include <evie/layer.h>
#include <evie/shader.h>
#include <evie/texture.h>
#include <evie/vertex_array.h>
#include <evie/vertex_buffer.h>
#include <evie/window.h>

class GameLayer final : public evie::Layer
{
public:
  static constexpr float DefaultPlayerSpeed = 5.0F;
  static constexpr float SprintPlayerSpeed = 10.0F;

  evie::Error
    Initialise(evie::IInputManager* input_manager, evie::ECSController* ecs_controller, evie::IWindow* window);
  void OnUpdate() override;
  void OnRender() override;
  void OnEvent(evie::Event& event) override;

private:
  void HandlePlayerCameraMovement(float delta_time);

  evie::Error SetupPlayer(float map_scale);

  evie::Error SetupFloor(float map_scale);

  evie::Error SetupSkybox(float map_scale);

  evie::Error SetupWalls(float map_scale);

  evie::Error SetupDanDan();

  // Player camera
  evie::FPSCamera player_camera_;

  // Last frame time stamp
  float last_frame_{ 0.0F };

  // Input manager
  evie::IInputManager* input_manager_{ nullptr };

  // ECS controller
  evie::ECSController* ecs_{ nullptr };

  // Window interface
  evie::IWindow* window_{ nullptr };

  // Floor Texture
  evie::Texture2D floor_texture_;

  // MeshComponent ID
  evie::ComponentID<evie::MeshComponent> mesh_cid_{ 0 };

  // TransformComponent ID
  evie::ComponentID<evie::TransformComponent> transform_cid_{ 0 };

  // FollowTargetComponent ID
  evie::ComponentID<FollowTargetComponent> follow_target_cid_{ 0 };

  // FollowerComponent ID
  evie::ComponentID<FollowerComponent> follower_cid_{ 0 };

  // ProjectComponent ID
  evie::ComponentID<ProjectileComponent> projectile_cid_{0};

  // VelocityComponent ID
  evie::ComponentID<VelocityComponent> velocity_cid_{0};

  // EnemyComponent ID
  evie::ComponentID<EnemyComponent> enemy_cid_{0};

  // Floor Vertex Shader
  evie::VertexShader floor_vertex_shader_;

  // Floor Fragment Shader
  evie::FragmentShader floor_fragment_shader_;

  // Render System
  Renderer* renderer_{ nullptr };

  // Follow System
  FollowSystem* follower_system_{ nullptr };

  // Projectile System
  ProjectileSystem* projectile_system_{ nullptr };

  // Physics System
  PhysicsSystem* physics_system_{nullptr};

  // DanDan System
  DanDanSystem* dandan_system_{nullptr};

  // Show cursor
  bool enable_cursor_{ false };

  // Player sprinting
  bool sprint_{ false };

  // Player entity
  evie::Entity* player_entity_{ nullptr };

  // Followers follow
  bool follow_on_{false};
};

#endif// !INCLUDE_GAME_LAYER_HPP_