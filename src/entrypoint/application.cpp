#include "evie/application.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "window/debug_layer.h"
#include "window/event_manager.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include "window/window.h"
#include <GLFW/glfw3.h>

namespace {
class TestLayer final : public evie::Layer
{
public:
  explicit TestLayer(int ind = 0) : ind_(ind) {}
  void OnUpdate() override {}
  void OnEvent([[maybe_unused]] evie::Event& event) override
  {
    if (event.IsInCategory(evie::EventCategoryBitmask::MouseButton)) {
      if (event.GetEventType() == evie::EventType::MouseButtonPressed) {
        EV_INFO("TestLayer_{} {}", ind_, event.ToString());
      }
    }
  }
  ~TestLayer() {}
  int ind_;
};

class TestLayer2 final : public evie::Layer
{
public:
  explicit TestLayer2(int ind = 0) : ind_(ind) {}
  void OnUpdate() override {}
  void OnEvent([[maybe_unused]] evie::Event& event) override
  {
    if (event.IsInCategory(evie::EventCategoryBitmask::Keyboard)) {
      if (event.GetEventType() == evie::EventType::KeyPressed) {
        EV_INFO("TestLayer_{} {}", ind_, event.ToString());
      }
    }
  }
  ~TestLayer2() {}
  int ind_;
};
}// namespace

namespace evie {
void Application::Run()
{
  // Logging
  LoggingManager::Init();
  EV_INFO("Logging Initialised");

  // Window
  WindowProperties props;
  props.name = "Evie";
  props.dimensions.width = 640;
  props.dimensions.height = 480;
  Window window(props);
  Error err = window.Initialise();

  // Layers
  DebugLayer debug_layer(window.GetGLFWWindow());
  TestLayer layer(1);
  TestLayer2 layer_2(2);
  LayerQueue layer_queue;
  layer_queue.PushBack(layer);
  layer_queue.PushBack(layer_2);
  layer_queue.PushBack(debug_layer);

  // Event System
  EventManager event_manager(layer_queue);
  event_manager.SubscribeToEventType(EventType::WindowClose, [this]([[maybe_unused]] const Event& event) {
    CloseWindow();
  });

  // TODO: Get rid of this and just construct window with the Event Manager
  if (err.Good()) {
    err = window.RegisterEventManager(event_manager);
  }

  if (err.Good()) {
    while (running_) {
      window.PollEvents();
      for (const auto& layer_wrapper : layer_queue) {
        layer_wrapper.layer->OnUpdate();
      }
      window.SwapBuffers();
    }
  }

  // Print error on exit if it's bad
  if (err.Bad()) {
    EV_WARN(err.Message());
  }
}
}// namespace evie
