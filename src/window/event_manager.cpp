#include "window/event_manager.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include <memory>
#include <ranges>

namespace evie {

// Although trivial these are defined here so that they don't become inline.
// An undefined reference to these functions will occur in the vtable if we don't
EventManager::EventManager(LayerQueue& layer_queue) : layer_queue_(layer_queue) {}
EventManager::~EventManager() {}

void EventManager::OnEvent(Event& event)
{
  EV_INFO(event.ToString());

  // Is this unnecessary? Currently used to prioritise individual event captures like closing the window.
  auto type_sub_it = type_subscribers_.find(event.GetEventType());
  if (type_sub_it != type_subscribers_.end()) {
    for (const auto& callback : type_sub_it->second) {
      event.handled_ = callback(event);
      if (event.handled_) {
        return;
      }
    }
  }

  // Iterate over layers in reverse and give events to the layers
  for (auto& layer_wrapper : std::ranges::views::reverse(layer_queue_)) {
    layer_wrapper.layer->OnEvent(event);
    if (event.handled_) {
      return;
    }
  }
}

void EventManager::SubscribeToEventType(EventType event_type, const std::function<bool(const Event&)>& callback)
{
  auto pair = type_subscribers_.emplace(event_type, std::vector<std::function<bool(const Event&)>>{ callback });
  if (!pair.second) {
    pair.first->second.push_back(callback);
  }
}

}// namespace evie