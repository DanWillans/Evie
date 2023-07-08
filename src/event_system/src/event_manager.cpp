#include "event_system/event_manager.h"
#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"
#include <memory>

namespace evie {

void EventManager::OnEvent(std::unique_ptr<Event> event)
{
  if (event->IsInCategory(EventCategoryBitmask::Keyboard)) {
    EV_INFO(event->ToString());
  } else if (event->IsInCategory(EventCategoryBitmask::Mouse)) {
    EV_INFO(event->ToString());
  }
}

}// namespace evie