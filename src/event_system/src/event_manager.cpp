#include "event_system/event_manager.h"
#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "evie/logging.h"
#include <memory>

namespace evie {

// Although trivial these are defined here so that they don't become inline.
// An undefined reference to these functions will occur in the vtable if we don't
IEventListener::IEventListener() {}
IEventListener::~IEventListener() {}

EventManager::EventManager() {}
EventManager::~EventManager() {}

std::unique_ptr<IEventListener> CreateEventManager() { return std::make_unique<EventManager>(); }

void EventManager::OnEvent(const Event& event)
{
  EV_INFO(event.ToString());

  // Is this unnecessary?
  // What if we just deliver events to layers? What about closing windows though?
  auto type_sub_it = type_subscribers_.find(event.GetEventType());
  if (type_sub_it != type_subscribers_.end()) {
    for (const auto& callback : type_sub_it->second) {
      callback(event);
    }
  }
}

void EventManager::SubscribeToEventType(EventType event_type, const std::function<void(const Event&)>& callback)
{
  auto pair = type_subscribers_.emplace(event_type, std::vector<std::function<void(const Event&)>>{ callback });
  if (!pair.second) {
    pair.first->second.push_back(callback);
  }
}

}// namespace evie