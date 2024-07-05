#ifndef EVIE_EVENT_MANAGER_H_
#define EVIE_EVENT_MANAGER_H_

#include <functional>
#include <memory>

#include "evie/core.h"
#include "evie/events.h"
#include "evie/input_manager.h"
#include "window/layer_queue.h"

namespace evie {

/**
 * @brief A class that manages all input events into the engine.
 *
 */
class EventManager
{
public:
  EVIE_API explicit EventManager(LayerQueue& layer_queue, IInputManager* input_manager);
  EVIE_API ~EventManager();
  void EVIE_API OnEvent(Event& event);
  /**
   * @brief Subscribe to specific event types. The user supplied callback will be called whenever the specificed
   * EventType is there.
   *
   * @param event_type The EventType to listen to
   * @param callback The callback to call when the specific event_type is found
   */
  void EVIE_API SubscribeToEventType(EventType event_type, const std::function<void(Event&)>& callback);

private:
  std::unordered_map<EventType, std::vector<std::function<void(Event&)>>> type_subscribers_;
  LayerQueue& layer_queue_;
  IInputManager* input_manager_{ nullptr };
};

}// namespace evie

#endif// !EVIE_EVENT_MANAGER_H_