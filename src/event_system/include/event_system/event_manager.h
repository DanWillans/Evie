#ifndef EVIE_EVENT_MANAGER_H_
#define EVIE_EVENT_MANAGER_H_

#include <functional>
#include <memory>

#include "event_system/events.h"
#include "evie/core.h"
#include "evie/error.h"

namespace evie {

class EVIE_API IEventListener
{
public:
  IEventListener() = default;
  IEventListener(const IEventListener&) = default;
  IEventListener(IEventListener&&) = default;
  IEventListener& operator=(IEventListener&&) = default;
  IEventListener& operator=(const IEventListener&) = default;
  virtual ~IEventListener() = default;

  virtual void OnEvent(const Event& event) = 0;
};

/**
 * @brief A class that manages all input events into the engine.
 *
 */
class EventManager final : public IEventListener
{
public:
  void EVIE_API OnEvent(const Event& event) override;

  /**
   * @brief Subscribe to specific event types. The user supplied callback will be called whenever the specificed
   * EventType is there.
   *
   * @param event_type The EventType to listen to
   * @param callback The callback to call when the specific event_type is found
   */
  void EVIE_API SubscribeToEventType(EventType event_type, const std::function<void(const Event&)>& callback);

  ~EventManager() override = default;

private:
  std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> type_subscribers_;
};
}// namespace evie

#endif// !EVIE_EVENT_MANAGER_H_