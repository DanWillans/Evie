
#include <evie/ecs/system_manager.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include "evie/application.h"
#include "evie/camera.h"
#include "evie/default_models.h"
#include "evie/ecs/components/transform.hpp"
#include "evie/ecs/components/velocity.hpp"
#include "evie/ecs/ecs_controller.hpp"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/evie.h"
#include "evie/ids.h"
#include "evie/indices_array.h"
#include "evie/input.h"
#include "evie/input_manager.h"
#include "evie/key_events.h"
#include "evie/layer.h"
#include "evie/logging.h"
#include "evie/mouse_events.h"
#include "evie/shader.h"
#include "evie/shader_program.h"
#include "evie/texture.h"
#include "evie/types.h"
#include "evie/vertex_array.h"
#include "evie/vertex_buffer.h"

#include "GLFW/glfw3.h"

// NOLINTBEGIN
namespace {
glm::vec3 cubePositions[] = { glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(2.0f, 5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f, 3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f, 2.0f, -2.5f),
  glm::vec3(1.5f, 0.2f, -1.5f),
  glm::vec3(-1.3f, 1.0f, -1.5f) };

class PhysicsSystem : public evie::System
{
private:
  void Update(const float& delta_time)
  {
    for (const auto& entity : entities) {
      auto& translate = entity.GetComponent(transform_component_id_);
      const auto& velocity = entity.GetComponent(velocity_component_id_);
      translate.position.x += delta_time * velocity.velocity.x;
      translate.position.y += delta_time * velocity.velocity.y;
      translate.position.z += delta_time * velocity.velocity.z;
    }
  }
  evie::ComponentID<evie::TransformRotationComponent> transform_component_id_{ 0 };
  evie::ComponentID<evie::VelocityComponent> velocity_component_id_{ 1 };
};

class RenderCubeSystem : public evie::System
{
public:
  RenderCubeSystem(evie::Camera* camera, evie::ShaderProgram* shader_program)
    : camera_(camera), shader_program_(shader_program)
  {}

private:
  void Update(const float& delta_time)
  {
    std::ignore = delta_time;
    int i = 0;
    for (const auto& entity : entities) {
      evie::mat4 model(1.0f);
      const auto& translate = entity.GetComponent(transform_component_id);
      // This moves the object to where we want it in world space.
      model = glm::translate(model, translate.position);
      // This rotates the object to where we want it in the world space.
      auto rot_mat4 = glm::eulerAngleXYZ(translate.rotation.x, translate.rotation.y, translate.rotation.z);
      model = model * rot_mat4;
      if (i % 3 == 0) {
        model =
          glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f), evie::vec3(1.0f, 0.3f, 0.5f));
      }
      i++;
      shader_program_->SetMat4("model", glm::value_ptr(model));
      evie::mat4 view = camera_->GetViewMatrix();
      shader_program_->SetMat4("view", glm::value_ptr(view));
      // This sets up the projection. What's our FoV? What's our aspect ratio?
      evie::mat4 projection = glm::perspective(glm::radians(camera_->field_of_view), 800.0f / 600.0f, 0.1f, 100.0f);
      shader_program_->SetMat4("projection", glm::value_ptr(projection));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }

  evie::ComponentID<evie::TransformRotationComponent> transform_component_id{ 0 };
  evie::Camera* camera_;
  evie::ShaderProgram* shader_program_;
};
}// namespace

class GameLayer : public evie::Layer
{
public:
  GameLayer() = default;
  explicit GameLayer(const evie::IInputManager* input_manager, evie::IWindow* window, evie::ECSController* ecs)
    : input_manager_(input_manager), window_(window), ecs_(ecs)
  {}

  evie::Error Initialise()
  {
    // ----- Textures -----
    evie::Error err = face_texture_.Initialise(
      "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\awesomeface.png");
    if (err.Good()) {
      err = container_texture_.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\container.jpg",
        true);
    }
    // ----- Shaders -----
    if (err.Good()) {
      err = vertex_shader_.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\vertex_shader.vs");
    }
    if (err.Good()) {
      err = fragment_shader_.Initialise("C:\\Users\\willa\\devel\\Evie\\shaders\\fragment_shader.fs");
    }
    if (err.Good()) {
      err = shader_program_.Initialise(&vertex_shader_, &fragment_shader_);
    }
    shader_program_.Use();
    // Setup shader uniforms
    shader_program_.SetInt("ourTexture1", 0);
    shader_program_.SetInt("ourTexture2", 1);
    // Set texture slots
    face_texture_.SetSlot(1);
    container_texture_.SetSlot(0);

    // ----- Initialise Vertex Objects -----
    if (err.Good()) {
      evie::BufferLayout layout;
      layout.stride = 5;
      layout.type = evie::VertexDataType::Float;
      layout.layout_sizes = { 3, 2 };
      err = vertex_buffer_.Initialise(evie::default_models::cube, layout);
    }

    if (err.Good()) {
      vertex_array_.Initialise();
      err = vertex_array_.AssociateVertexBuffer(vertex_buffer_);
    }

    // Disable cursor initially
    window_->DisableCursor();

    // Register components with ECS
    transform_component_id_ = ecs_->RegisterComponent<evie::TransformRotationComponent>();
    velocity_component_id_ = ecs_->RegisterComponent<evie::VelocityComponent>();

    // Register system with ECS
    evie::SystemSignature render_signature;
    render_signature.SetComponent(transform_component_id_);
    render_cube_system_id_ = ecs_->RegisterSystem<RenderCubeSystem>(render_signature, &camera_, &shader_program_);
    cube_render_ = &ecs_->GetSystem(render_cube_system_id_);

    evie::SystemSignature physics_signature;
    physics_signature.SetComponent(transform_component_id_);
    physics_signature.SetComponent(velocity_component_id_);
    physics_system_id_ = ecs_->RegisterSystem<PhysicsSystem>(physics_signature);
    physics_system_ = &ecs_->GetSystem(physics_system_id_);


    // Create 10 cube entities
    for (int i = 0; i < 10; ++i) {
      auto entity = ecs_->CreateEntity();
      if (entity.Good()) {
        cube_entities_.push_back(*entity);
        float angle = 20.0f * static_cast<float>(i);
        evie::TransformRotationComponent transform;
        transform.rotation = evie::vec3(1.0f, 0.3f, 0.5f) * glm::radians(angle);
        transform.position = cubePositions[i];
        err = entity->AddComponent(transform_component_id_, transform);
        if (err.Bad()) {
          break;
        }
        evie::VelocityComponent velocity;
        velocity.velocity.x = 0;
        velocity.velocity.y = -1;
        velocity.velocity.z = 0;
        err = entity->AddComponent(velocity_component_id_, velocity);
        if (err.Bad()) {
          break;
        }
      }
    }

    // Create

    // Initialise camera speed
    camera_.camera_speed = 10;
    return err;
  }

  void OnRender() override
  {
    // Update mixer for shader
    shader_program_.SetFloat("mixer", mixer_);
    // Bind the Vertex Array that associates our cube models
    vertex_array_.Bind();
    cube_render_->UpdateSystem(0.0F);
  }

  void OnUpdate() override
  {
    // Calculate some delta times between last OnUpdate call
    float current_frame = static_cast<float>(glfwGetTime());
    float delta_time = current_frame - last_frame_;
    last_frame_ = current_frame;

    physics_system_->UpdateSystem(delta_time);
    auto& rotation = cube_entities_[0].GetComponent(transform_component_id_).rotation;
    rotation.y += static_cast<float>(glfwGetTime()) * glm::radians(1.0f);

    // Handle camera translation
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

  void OnEvent([[maybe_unused]] evie::Event& event) override
  {
    // Process mouse move events for camera rotation
    if (!cursor_enabled_) {
      if (event.GetEventType() == evie::EventType::MouseMoved) {
        const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
        camera_.Rotate(mouse_event->GetMousePosition());
        event.handled = true;
        return;
      }
    }

    // Process mouse scrolled events for camera fov
    if (event.GetEventType() == evie::EventType::MouseScrolled) {
      const auto* const scrolled_event = event.Cast<evie::MouseScrolledEvent>();
      auto& fov = camera_.field_of_view;
      fov -= static_cast<float>(scrolled_event->GetScrollOffset().y_offset);
      if (fov < 1.0f) {
        fov = 1.0f;
      }
      if (fov > 45.0f) {
        fov = 45.0f;
      }
      event.handled = true;
      return;
    }

    // Process j/k for mixer values
    if (event.GetEventType() == evie::EventType::KeyPressed) {
      const auto* const key_event = event.Cast<evie::KeyPressedEvent>();
      if (key_event->IsKeyCode(evie::KeyCode::J)) {
        mixer_ += 0.01f;
        if (mixer_ > 1.0f) {
          mixer_ = 1.0f;
        }
      } else if (key_event->IsKeyCode(evie::KeyCode::K)) {
        mixer_ -= 0.01f;
        if (mixer_ < 0.0f) {
          mixer_ = 0.0f;
        }
      } else if (key_event->IsKeyCode(evie::KeyCode::Escape)) {
        if (cursor_enabled_) {
          window_->DisableCursor();
          cursor_enabled_ = false;
        } else {
          window_->EnableCursor();
          cursor_enabled_ = true;
        }
      } else if (key_event->IsKeyCode(evie::KeyCode::Up)) {
        camera_.camera_speed += 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::Down)) {
        camera_.camera_speed -= 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::R)) {
        camera_.ResetCameraPosition();
      }
    }
  }

  void Shutdown() override
  {
    vertex_array_.Destroy();
    indices_array_.Destroy();
    vertex_buffer_.Destroy();
    shader_program_.Destroy();
    container_texture_.Destroy();
    face_texture_.Destroy();
    // Destroy entities
    for (auto& entity : cube_entities_) {
      entity.Destroy();
    }
  }


private:
  evie::Camera camera_;
  evie::Texture2D face_texture_;
  evie::Texture2D container_texture_;
  evie::VertexShader vertex_shader_;
  evie::ShaderProgram shader_program_;
  evie::FragmentShader fragment_shader_;
  evie::VertexBuffer vertex_buffer_;
  evie::IndicesArray indices_array_;
  evie::VertexArray vertex_array_;
  float last_frame_ = 0.0f;
  float mixer_{ 0.2f };
  bool cursor_enabled_{ false };
  const evie::IInputManager* input_manager_{ nullptr };
  evie::IWindow* window_{ nullptr };
  evie::ECSController* ecs_{ nullptr };
  std::vector<evie::Entity> cube_entities_;
  evie::ComponentID<evie::TransformRotationComponent> transform_component_id_{ 0 };
  evie::ComponentID<evie::VelocityComponent> velocity_component_id_{ 0 };
  evie::SystemID<RenderCubeSystem> render_cube_system_id_{ 0 };
  evie::SystemID<PhysicsSystem> physics_system_id_{ 0 };
  RenderCubeSystem* cube_render_{ nullptr };
  PhysicsSystem* physics_system_{ nullptr };
};


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
    if (err.Good()) {
      APP_INFO("Creating GameLayer");
      t_layer_ = GameLayer(GetInputManager(), GetWindow(), GetECSController());
      APP_INFO("Initialising layer");
      err = t_layer_.Initialise();
      if (err.Good()) {
        APP_INFO("Pushing layer");
        PushLayerFront(t_layer_);
      }
    }
    return err;
  }
  ~Sandbox() override = default;

private:
  GameLayer t_layer_;
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

// NOLINTEND