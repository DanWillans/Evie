#include "window/event_manager.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "evie/input_manager.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include <memory>

namespace evie {
// Although trivial these are defined here so that they don't become inline.
// An undefined reference to these functions will occur in the vtable if we don't
EventManager::EventManager(LayerQueue& layer_queue, IInputManager* input_manager)
  : layer_queue_(layer_queue), input_manager_(input_manager)
{}
EventManager::~EventManager() {}

void EventManager::OnEvent(Event& event)
{
  // The input manager needs visibility on all events to store the state of the event.
  input_manager_->RegisterInput(event);


  // Is this unnecessary? Currently used to prioritise individual event captures like closing the window.
  auto type_sub_it = type_subscribers_.find(event.GetEventType());
  if (type_sub_it != type_subscribers_.end()) {
    for (const auto& callback : type_sub_it->second) {
      callback(event);
      if (event.handled) {
        return;
      }
    }
  }

  // Iterate over layers in reverse and give events to the layers
  for (LayerQueue::ReverseIterator layer_it = layer_queue_.rbegin(); layer_it != layer_queue_.rend(); ++layer_it) {
    layer_it->layer->OnEvent(event);
    if (event.handled) {
      return;
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