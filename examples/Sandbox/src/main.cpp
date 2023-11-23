#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "evie/application.h"
#include "evie/camera.h"
#include "evie/default_models.h"
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
}

class GameLayer : public evie::Layer
{
public:
  GameLayer() = default;
  explicit GameLayer(const evie::IInputManager* input_manager, evie::IWindow* window)
    : input_manager_(input_manager), window_(window)
  {}

  evie::Error Initialise()
  {
    // ----- Textures -----
    evie::Error err = face_texture_.Initialise(
      "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\awesomeface.png",
      evie::ImageFormat::RGBA);
    if (err.Good()) {
      err = container_texture_.Initialise(
        "C:\\Users\\willa\\devel\\Evie\\out\\install\\windows-msvc-debug-developer-mode\\textures\\container.jpg",
        evie::ImageFormat::RGB,
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

    // ----- Initialise Vertex Objects -----
    if (err.Good()) {
      evie::BufferLayout layout;
      layout.stride = 5;
      layout.type = evie::VertexDataType::Float;
      layout.layout_sizes = { 3, 2 };
      err = vertex_buffer_.Initialise(evie::default_models::cube, layout);
    }

    if (err.Good()) {
      indices_array_.Initialise(evie::default_models::cube_indices);
      vertex_array_.Initialise();
      vertex_array_.AssociateIndicesArray(indices_array_);
      err = vertex_array_.AssociateVertexBuffer(vertex_buffer_);
    }

    // Disable cursor initially
    window_->DisableCursor();

    return err;
  }

  void OnRender() override
  {
    // Set texture slots
    face_texture_.SetSlot(0);
    container_texture_.SetSlot(1);
    // Update mixer for shader
    shader_program_.SetFloat("mixer", mixer_);
    // Bind the Vertex Array that associates our cube models
    vertex_array_.Bind();
    // Iterate over our cube positions and draw them
    for (unsigned int i = 0; i < 10; ++i) {
      evie::mat4 model = evie::mat4(1.0f);
      // This translates the object to the position in the world that we want it.
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * static_cast<float>(i);
      // This rotates the object to where we want it in the world space.
      if (i % 3 == 0) {
        model =
          glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f), evie::vec3(1.0f, 0.3f, 0.5f));
      } else {
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      }
      shader_program_.SetMat4("model", glm::value_ptr(model));
      evie::mat4 view = camera_.GetViewMatrix();
      shader_program_.SetMat4("view", glm::value_ptr(view));
      // This sets up the projection. What's our FoV? What's our aspect ratio?
      evie::mat4 projection = glm::perspective(glm::radians(camera_.field_of_view), 800.0f / 600.0f, 0.1f, 100.0f);
      shader_program_.SetMat4("projection", glm::value_ptr(projection));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }

  void OnUpdate() override
  {
    // Calculate some delta times between last OnUpdate call
    float current_frame = static_cast<float>(glfwGetTime());
    float delta_time = current_frame - last_frame_;
    last_frame_ = current_frame;

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
      } else if (key_event->IsKeyCode(evie::KeyCode::Up)){
        camera_.camera_speed += 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::Down)){
        camera_.camera_speed -= 0.05f;
      } else if (key_event->IsKeyCode(evie::KeyCode::R)){
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
      t_layer_ = GameLayer(GetInputManager(), GetWindow());
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
