#include "evie/application.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "window/event_manager.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include "window/window.h"

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
        [[maybe_unused]] auto mouse_event = static_cast<evie::MousePressedEvent*>(&event);
      } else {
        EV_INFO("TestLayer_{} {}", ind_, event.ToString());
        [[maybe_unused]] auto mouse_event = static_cast<evie::MousePressedEvent*>(&event);
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
        [[maybe_unused]] auto mouse_event = static_cast<evie::KeyPressedEvent*>(&event);
      } else {
        EV_INFO("TestLayer_{} {}", ind_, event.ToString());
        [[maybe_unused]] auto mouse_event = static_cast<evie::KeyReleasedEvent*>(&event);
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
  LoggingManager::Init();
  EV_INFO("Logging Initialised");
  // Setup window properties and create window
  WindowProperties props;
  props.name = "Evie";
  props.dimensions.width = 640;
  props.dimensions.height = 480;
  Window window(props);

  TestLayer layer(1);
  TestLayer2 layer_2(2);
  LayerQueue layer_queue;
  layer_queue.PushBack(layer);
  layer_queue.PushBack(layer_2);
  EventManager event_manager(layer_queue);
  event_manager.SubscribeToEventType(EventType::WindowClose, [this]([[maybe_unused]] const Event& event) {
    CloseWindow();
    return true;
  });

  // TODO: Get rid of this and just construct window with the Event Manager
  Error err = window.RegisterEventManager(event_manager);

  // Initialise the window
  if (err.Good()) {
    // Update the window
    err = window.Initialise();
    // Update the layers
    for (const auto& layer_wrapper : layer_queue) {
      layer_wrapper.layer->OnUpdate();
    }
  }

  // Main run loop
  while (running_) {
    window.Update();
  }

  // Print error on exit if it's bad
  if (err.Bad()) {
    EV_WARN(err.Message());
  }
}
}// namespace evie
