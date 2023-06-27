#include "bitmask_operators.h"

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

enum class EventCategoryBitmask {
  None = 0,
  Application = 1 << 0,
  Input = 1 << 1,
  Keyboard = 1 << 2,
  Mouse = 1 << 3,
  MouseButton = 1 << 4,
};

// Template specialise EventCategoryBitmask to allow it to act as a bitmask
template<> struct enable_bitmask_operators<EventCategoryBitmask>
{
  static constexpr bool enable = true;
};

class EVIE_API Event
{
  virtual EventType GetEventType() const = 0;
  virtual EventCategoryBitmask GetCategoryFlags() const = 0;
  virtual std::string ToString() const { return GetName(); }

  bool IsInCategory(EventCategoryBitmask category) { return GetCategoryFlags() & category; }

private:
  // A flag that says whether this event has been handled.
  bool handled_;
};


}// namespace evie