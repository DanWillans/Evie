#include "evie/application.h"
#include "event_system/event_manager.h"
#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"
#include "window/window.h"

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

  // Setup event manager and register it as a listener with the window
  EventManager event_manager;
  event_manager.SubscribeToEventType(
    EventType::WindowClose, [this]([[maybe_unused]] const Event& event) { CloseWindow(); });
  Error err = window.RegisterEventListener(&event_manager);

  // Initialise the window
  if (err.Good()) {
    err = window.Initialise();
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
