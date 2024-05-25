#include "game_layer.hpp"

#include <GLFW/glfw3.h>

#include <evie/ecs/components/Transform.hpp>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/system_signature.hpp>
#include <evie/error.h>
#include <evie/input.h>
#include <evie/input_manager.h>
#include <evie/mouse_events.h>
#include <evie/shader.h>
#include <evie/vertex_buffer.h>

evie::Error GameLayer::Initialise(evie::IInputManager* input_manager, evie::ECSController* ecs_controller)
{
  evie::Error err = evie::Error::OK();
  // Initialise our variables
  input_manager_ = input_manager;
  if (!input_manager_) {
    return evie::Error{ "Invalid input manager" };
  }

  ecs_ = ecs_controller;
  if (ecs_ == nullptr) {
    return evie::Error{ "Invalid ECS" };
  }

  // Let's make a floor
  // We're going to do this by creating a 2d plane and mapping a texture to it.
  // We can then scale the plane in size to increase the floor area.
  // clang-format off
  const std::vector<float> floor_model = {
  // x    y     z    u    v
    0.0, 0.0,  0.0, 0.0, 0.0,
    0.0, 0.0, -1.0, 0.0, 1.0,
    1.0, 0.0,  0.0, 1.0, 0.0,
    0.0, 0.0, -1.0, 0.0, 1.0,
    1.0, 0.0,  1.0, 1.0, 1.0,
    1.0, 0.0,  0.0, 1.0, 0.0
        // first triangle
    //  0.5f,  0.5f, 0.0f, 1.0, 1.0, // top right
    //  0.5f, -0.5f, 0.0f, 1.0, 0.0,// bottom right
    // -0.5f,  0.5f, 0.0f, 0.0, 1.0, // top left 
    // // second triangle
    //  0.5f, -0.5f, 0.0f, 1.0, 0.0, // bottom right
    // -0.5f, -0.5f, 0.0f, 0.0, 0.0, // bottom left
    // -0.5f,  0.5f, 0.0f, 0.0, 1.0  // top left
  };
  // clang-format on

  // Vertex Shader
  if (err.Good()) {

    err = floor_vertex_shader_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\vertex_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = floor_fragment_shader_.Initialise(R"(C:\Users\willa\devel\Evie\shaders\fragment_shader.fs)");
  }

  // Floor texture
  if (err.Good()) {
    err = floor_texture_.Initialise(
      R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\textures\ty.png)", true);
  }

  // Create floor mesh component
  evie::MeshComponent floor_mesh_component;
  // We need to create the layout for our model/vertex data
  evie::BufferLayout layout;
  layout.stride = 5;
  layout.type = evie::VertexDataType::Float;
  layout.layout_sizes = { 3, 2 };
  if (err.Good()) {
    // Initialise the vertex buffer
    err = floor_mesh_component.model_data.Initialise(floor_model, layout);
  }
  if (err.Good()) {
    // Associate the vertex buffer with the vertex array
    floor_mesh_component.vertex_array.Initialise();
    err = floor_mesh_component.vertex_array.AssociateVertexBuffer(floor_mesh_component.model_data);
  }
  if (err.Good()) {
    // Initialise the shader program with our vert/frag shaders
    err = floor_mesh_component.shader_program.Initialise(&floor_vertex_shader_, &floor_fragment_shader_);
  }
  // Now setup the texture slots and bind them to our shader program.
  floor_mesh_component.shader_program.Use();
  floor_mesh_component.shader_program.SetInt("ourTexture1", 0);
  floor_texture_.SetSlot(0);

  // Let's use ECS to add data to our models
  // Register our componenets
  mesh_cid_ = ecs_->RegisterComponent<evie::MeshComponent>();
  transform_cid_ = ecs_->RegisterComponent<evie::TransformRotationComponent>();

  // Register our ecs 
  evie::SystemSignature signature;
  signature.SetComponent(mesh_cid_);
  signature.SetComponent(transform_cid_);
  auto sys_id = ecs_->RegisterSystem<Renderer>(signature);
  renderer_ = &(ecs_->GetSystem(sys_id));
  renderer_->Initialise(mesh_cid_, transform_cid_, &player_camera_);

  // Create floor
  auto floor_entity = ecs_->CreateEntity();
  if (err.Good()) {
    err = floor_entity->AddComponent(mesh_cid_, floor_mesh_component);
    if (err.Good()) {
      err = floor_entity->AddComponent(transform_cid_);
    }
  }

  return err;
}
void GameLayer::OnUpdate()
{
  // Calculate some delta times between last OnUpdate call
  // TODO - Would it be nice if we didn't have to handle delta_time ourselves?
  float current_frame = static_cast<float>(glfwGetTime());
  float delta_time = current_frame - last_frame_;
  last_frame_ = current_frame;

  HandlePlayerCameraMovement(delta_time);
}
void GameLayer::OnRender() { renderer_->Iterate(); }
void GameLayer::OnEvent(evie::Event& event)
{
  if (event.GetEventType() == evie::EventType::MouseMoved) {
    const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
    player_camera_.Rotate(mouse_event->GetMousePosition());
    event.handled = true;
    return;
  }
}

void GameLayer::HandlePlayerCameraMovement(float delta_time)
{
  // Handle camera translation on update
  if (input_manager_->IsKeyPressed(evie::KeyCode::W)) {
    player_camera_.MoveForwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::S)) {
    player_camera_.MoveBackwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::A)) {
    player_camera_.MoveLeft(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::D)) {
    player_camera_.MoveRight(delta_time);
  }
}
