#include <memory>

#include "evie/application.h"
#include "evie/camera.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/evie.h"
#include "evie/input.h"
#include "evie/input_manager.h"
#include "evie/layer.h"
#include "evie/logging.h"
#include "evie/mouse_events.h"
#include "evie/texture.h"
#include "evie/types.h"


class GameLayer : public evie::Layer
{
public:
  GameLayer() = default;
  explicit GameLayer(const evie::IInputManager* input_manager) : input_manager_(input_manager) {}

  evie::Error Initialise()
  {
    evie::Error err = face_texture_.Initialise("..\\textures\\container.jpg");
    if (err.Good()) {
      err = container_texture_.Initialise("..\\textures\\awesomeface.png");
    }
    return err;
  }

  void OnUpdate() override {}

  void OnEvent([[maybe_unused]] evie::Event& event) override
  {
    // Process mouse move events for camera rotation
    if (event.GetEventType() == evie::EventType::MouseMoved) {
      const auto* const mouse_event = event.Cast<evie::MouseMovementEvent>();
      camera_.Rotate(mouse_event->GetMousePosition());
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
    }
  }

private:
  evie::Camera camera_;
  evie::Texture2D face_texture_;
  evie::Texture2D container_texture_;
  const evie::IInputManager* input_manager_{ nullptr };
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
    evie::Error err = Initialise(props);
    if (err.Good()) {
      APP_INFO("Creating GameLayer");
      t_layer_ = GameLayer(GetInputManager());
      APP_INFO("Initialising layer");
      err = t_layer_.Initialise();
      if(err.Good()){
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
  std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
  evie::Error err = app->Init();
  if (err.Good()) {
    return app;
  } else {
    APP_INFO("App failed to initialise. Reason: {}", err.Message());
    return nullptr;
  }
}
