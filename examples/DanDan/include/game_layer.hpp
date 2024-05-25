#ifndef INCLUDE_GAME_LAYER_HPP_
#define INCLUDE_GAME_LAYER_HPP_

#include "render.hpp"
#include <evie/camera.h>
#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/error.h>
#include <evie/ids.h>
#include <evie/input_manager.h>
#include <evie/layer.h>
#include <evie/shader.h>
#include <evie/texture.h>
#include <evie/vertex_array.h>
#include <evie/vertex_buffer.h>

class GameLayer final : public evie::Layer
{
public:
  evie::Error Initialise(evie::IInputManager* input_manager, evie::ECSController* ecs_controller);
  void OnUpdate() override;
  void OnRender() override;
  void OnEvent(evie::Event& event) override;

private:
  void HandlePlayerCameraMovement(float delta_time);

  // Player camera
  evie::Camera player_camera_;

  // Last frame time stamp
  float last_frame_{ 0.0F };

  // Input manager
  evie::IInputManager* input_manager_{ nullptr };

  // ECS controller
  evie::ECSController* ecs_{ nullptr };

  // Floor Texture
  evie::Texture2D floor_texture_;

  // MeshComponent ID
  evie::ComponentID<evie::MeshComponent> mesh_cid_{0};

  // TransformComponent ID
  evie::ComponentID<evie::TransformRotationComponent> transform_cid_{0};

  // Floor Vertex Shader
  evie::VertexShader floor_vertex_shader_;

  // Floor Fragment Shader
  evie::FragmentShader floor_fragment_shader_;

  // Render System
  Renderer* renderer_;
};

#endif// !INCLUDE_GAME_LAYER_HPP_