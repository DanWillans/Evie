#include "evie/application.h"
#include "event_system/event_manager.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"
#include "window/window.h"

namespace evie {
void Application::Run()
{
  LoggingManager::Init();
  EV_INFO("Logging Initialised");
  Window window("Evie");
  EventManager event_manager;
  Error err = window.RegisterEventListener(&event_manager);
  if (err.Good()) {
    err = window.Initialise();
  }
  if (err.Bad()) {
    EV_WARN(err.Message());
  }
  while (true) {
    window.Update();
  }
  if (err.Bad()) {
    EV_WARN(err.Message());
  }
}
}// namespace evie
