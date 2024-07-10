#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "rendering/model.hpp"

#include <GLFW/glfw3.h>

#include <evie/application.h>
#include <evie/camera.h>
#include <evie/default_models.h>
#include <evie/ecs/components/mesh_component.hpp>
#include <evie/ecs/ecs_controller.hpp>
#include <evie/ecs/system_signature.hpp>
#include <evie/entrypoint.h>
#include <evie/error.h>
#include <evie/events.h>
#include <evie/input.h>
#include <evie/input_manager.h>
#include <evie/key_events.h>
#include <evie/mouse_events.h>
#include <evie/shader.h>
#include <evie/shader_program.h>
#include <evie/texture.h>
#include <evie/vertex_buffer.h>
#include <evie/window.h>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>

class ModelExample final : public evie::Layer
{
public:
  evie::Error
    Initialise(evie::IInputManager* input_manager, evie::ECSController* ecs_controller, evie::IWindow* window);
  void OnUpdate() override;
  void OnRender() override;
  void OnEvent(evie::Event& event) override;

private:
  void HandlePlayerCameraMovement(float delta_time);

  // Player camera
  evie::FPSCamera camera_;

  // Last frame time stamp
  float last_frame_{ 0.0F };

  // Input manager
  evie::IInputManager* input_manager_{ nullptr };

  // ECS controller
  evie::ECSController* ecs_{ nullptr };

  // Window interface
  evie::IWindow* window_{ nullptr };

  // Show cursor
  bool enable_cursor_{ false };

  // Testing - REMOVE
  evie::ShaderProgram model_prog;
  evie::Model backpack_model{
    R"(C:\Users\willa\devel\Evie\out\install\windows-msvc-debug-developer-mode\assets\models\backpack.obj)"
  };
};

evie::Error ModelExample::Initialise(evie::IInputManager* input_manager,
  evie::ECSController* ecs_controller,
  evie::IWindow* window)
{
  evie::Error err = evie::Error::OK();
  // Initialise our variables
  input_manager_ = input_manager;
  if (input_manager_ == nullptr) {
    return evie::Error{ "Invalid input manager" };
  }

  ecs_ = ecs_controller;
  if (ecs_ == nullptr) {
    return evie::Error{ "Invalid ECS" };
  }

  window_ = window;
  if (window_ == nullptr) {
    return evie::Error{ "Invalid window" };
  }

  // Disable cursor
  if (!enable_cursor_) {
    window_->DisableCursor();
  }

  // Vertex Shader
  evie::VertexShader vert_shader;
  evie::FragmentShader frag_shader;
  if (err.Good()) {
    err = vert_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\light_vert_diffuse_shader.vs)");
  }

  // Fragment Shader
  if (err.Good()) {
    err = frag_shader.Initialise(R"(C:\Users\willa\devel\Evie\shaders\light_frag_model.fs)");
  }

  if (err.Good()) {
    err = model_prog.Initialise(&vert_shader, &frag_shader);
  }

  if (err.Good()) {
    err = backpack_model.Initialise();
  }

  return err;
}

void ModelExample::OnUpdate()
{
  // Calculate some delta times between last OnUpdate call
  // TODO - Would it be nice if we didn't have to handle delta_time ourselves?
  auto current_frame = static_cast<float>(glfwGetTime());
  float delta_time = current_frame - last_frame_;
  last_frame_ = current_frame;

  HandlePlayerCameraMovement(delta_time);
}

void ModelExample::OnRender()
{
  model_prog.Use();
  // // Handle transforming the object first
  // // This moves the object to where we want it in world space.
  auto model = glm::mat4(1.0);
  // model = glm::translate(model, translate.position);
  // // APP_INFO("Render entity {} with position {}", entity.GetID().Get(), glm::to_string(translate.position));
  // // This rotates the object to where we want it in the world space.
  // model = model * glm::toMat4(translate.rotation);
  // model = glm::scale(model, translate.scale);

  // Bind VAO and Shader Program
  // auto& shader_program = mesh.shader_program;
  // shader_program.Use();
  if (model_prog.HasVec3("viewPos")) {
    model_prog.SetVec3("viewPos", camera_.GetPosition());
  }
  // mesh.vertex_array.Bind();

  // Set texture. Only one texture per mesh component atm.
  // mesh.texture.SetSlot(0);
  model_prog.SetVec3("directional_light.direction", { 0.0f, -1.0f, 0.0f });
  model_prog.SetVec3("directional_light.ambient", { 0.5f, 0.5f, 0.5f });
  model_prog.SetVec3("directional_light.diffuse", { 0.5f, 0.5f, 0.5f });
  model_prog.SetVec3("directional_light.specular", { 1.0f, 1.0f, 1.0f });
  model_prog.SetFloat("material.shininess", 225.0F);

  // Update uniforms in the shader program
  model_prog.SetMat4("model", glm::value_ptr(model));
  evie::mat4 view = camera_.GetViewMatrix();
  model_prog.SetMat4("view", glm::value_ptr(view));
  view = glm::inverseTranspose(view);
  model_prog.SetMat4("inverse_transpose_view", glm::value_ptr(view));
  // This sets up the projection. What's our FoV? What's our aspect ratio? Fix this to get from camera.
  constexpr float near_cull = 0.1F;
  constexpr float far_cull = 1000.0F;
  evie::mat4 projection =
    glm::perspective(glm::radians(camera_.field_of_view), window_->GetAspectRatio(), near_cull, far_cull);
  model_prog.SetMat4("projection", glm::value_ptr(projection));
  backpack_model.Draw(model_prog);
}

void ModelExample::OnEvent(evie::Event& event)
{
  if (event.GetEventType() == evie::EventType::MouseMoved) {
    // Don't rotate the camera if the cursor is enabled
    const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
    camera_.Rotate(mouse_event->GetMousePosition(), !enable_cursor_);
    event.handled = true;
    return;
  }

  if (event.GetEventType() == evie::EventType::KeyPressed) {
    const auto* const key_event = event.Cast<evie::KeyPressedEvent>();
    // Check if we should enable the cursor or not
    if (key_event->IsKeyCode(evie::KeyCode::Escape)) {
      enable_cursor_ = !enable_cursor_;
      if (enable_cursor_) {
        window_->EnableCursor();
      } else {
        window_->DisableCursor();
      }
      event.handled = true;
      return;
    }
  }
}

void ModelExample::HandlePlayerCameraMovement(float delta_time)
{
  // Handle camera translation on update
  if (input_manager_->IsKeyPressed(evie::KeyCode::W)) {
    camera_.MoveForwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::S)) {
    camera_.MoveBackwards(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::A)) {
    camera_.MoveLeft(delta_time);
  }
  if (input_manager_->IsKeyPressed(evie::KeyCode::D)) {
    camera_.MoveRight(delta_time);
  }
}


class Sandbox : public evie::Application
{
public:
  [[nodiscard]] evie::Error Init()
  {
    evie::WindowProperties props;
    props.name = "Sandbox";
    props.dimensions.width = 1260;
    props.dimensions.height = 720;
    // Always Initialise the engine before doing anything with it.
    APP_INFO("Initialiasing engine");
    evie::Error err = Initialise(props);
    // Set ImGui context from the engine.
    ImGui::SetCurrentContext(GetImGuiContext());
    if (err.Good()) {
      APP_INFO("Creating GameLayer");
      t_layer_ = std::make_unique<ModelExample>();
      APP_INFO("Initialising layer");
      err = t_layer_->Initialise(GetInputManager(), GetECSController(), GetWindow());
      if (err.Good()) {
        APP_INFO("Pushing layer");
        PushLayerFront(*t_layer_);
      }
    }
    return err;
  }
  ~Sandbox() override = default;

private:
  std::unique_ptr<ModelExample> t_layer_;
};

std::unique_ptr<evie::Application> CreateApplication()
{
  APP_INFO("Making Sandbox");
  std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
  APP_INFO("Initialising Sandbox");
  evie::Error err = app->Init();
  if (err.Good()) {
    APP_INFO("Sandbox Initialised Succesfully");
    return app;
  } else {
    APP_INFO("App failed to initialise. Reason: {}", err.Message());
    return nullptr;
  }
}
