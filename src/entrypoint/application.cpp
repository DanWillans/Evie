#include "evie/application.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"


namespace evie {
void Application::Run()
{
  LoggingManager::Init();
  EV_INFO("Logging Initialised");
  // KeyPressedEvent event(1, 2);
  MousePressedEvent event(MouseButton::Left);
  Event* eve = &event;
  if (eve->IsInCategory(EventCategoryBitmask::Keyboard)) { EV_INFO("Event is in keyboard category"); }
  if (eve->IsInCategory(EventCategoryBitmask::Input)){
    EV_INFO("Event is in input category");
  }
  if(eve->IsInCategory(EventCategoryBitmask::Mouse)){
    EV_INFO("Event is in mouse category");
  }
  if(eve->IsInCategory(EventCategoryBitmask::MouseButton)){
    EV_INFO("Event is in mouse button category - {}", eve->ToString());
  }
  while (true) {}
}
}// namespace evie
