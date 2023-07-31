#include "evie/application.h"
#include "event_system/event_manager.h"
#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"
#include "window/window.h"
#include "window/layer_queue.h"

#include <ranges>

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
  std::unique_ptr<IEventListener> event_manager = CreateEventManager();
  event_manager->SubscribeToEventType(
    EventType::WindowClose, [this]([[maybe_unused]] const Event& event) { CloseWindow(); });
  Error err = window.RegisterEventListener(event_manager.get());

  LayerQueue layer_queue;

  // Initialise the window
  if (err.Good()) {
    err = window.Initialise();
    // for(const auto& layer : std::ranges::views::reverse(layer_queue)){
      
    // }
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
