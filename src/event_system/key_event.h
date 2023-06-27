#include <sstream>

#include "events.h"

namespace evie {
class EVIE_API KeyEvent : public Event
{
public:
  int GetKeyCode() const { return key_code_ };

  EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Keyboard | EventCategoryBitmask::Input;
  }

protected:
  KeyEvent(int key_code) : key_code_(key_code) {}
  int key_code_;
};

class EVIE_API KeyPressedEvent final : public KeyEvent
{
public:
  static constexpr EventType type = EventType::KeyPressed;
  KeyPressedEvent(int key_code, int repeat_count) : key_code_(key_code), repeat_count_(repeat_count) {}

  constexpr EventType GetEventType() const override { return KeyPressedEvent::type; }

  int GetRepeatCount() const { return repeat_count_ };

  std::string ToString() const override
  {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << key_code_ << " (" << repeat_count << " repeats)";
    return ss.str();
  }

private:
  int repeat_count_;
};

class EVIE_API KeyReleasedEvent final : public KeyEvent
{
public:
  static constexpr EventType type = EventType::KeyReleased;
  KeyReleasedEvent(int key_code) : key_code_(key_code) {}

  constexpr EventType GetEventType() const override { return KeyReleasedEvent::type; }

  std::string ToString() const override
  {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << key_code_ << "\n";
    return ss.str();
  }
}

}// namespace evie