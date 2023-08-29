#ifndef EVIE_EVENT_SYSTEM_EVENTS_H_
#define EVIE_EVENT_SYSTEM_EVENTS_H_

#include <cstdint>
#include <string>

#include "utility/bitmask_operators.h"

#include "evie/core.h"

namespace evie {

enum class EventType {
  None = 0,
  WindowClose,
  WindowResize,
  WindowFocus,
  WindowLostFocus,
  WindowMoved,
  KeyPressed,
  KeyReleased,
  MouseMoved,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseScrolled
};

enum class EventCategoryBitmask : uint16_t {
  None = 0,
  Application = 1 << 0U,
  Input = 1 << 1U,
  Keyboard = 1 << 2U,
  Mouse = 1 << 3U,
  MouseButton = 1 << 4U,
};

// Template specialise EventCategoryBitmask to allow it to act as a bitmask
template<> struct enable_bitmask_operators<EventCategoryBitmask>
{
  static constexpr bool enable = true;
};

class EventManager;

class EVIE_API Event
{
public:
  [[nodiscard]] virtual EventType GetEventType() const = 0;
  [[nodiscard]] virtual EventCategoryBitmask GetCategoryFlags() const = 0;
  [[nodiscard]] virtual std::string ToString() const = 0;
  virtual ~Event() = default;

  [[nodiscard]] bool IsInCategory(EventCategoryBitmask category) const
  {
    return static_cast<bool>(GetCategoryFlags() & category);
  }

protected:
  friend EventManager;
  Event() = default;
  // A flag that says whether this event has been handled.
  bool handled_{ false };
};


}// namespace evie

#endif// !EVIE_EVENT_SYSTEM_EVENTS_H_