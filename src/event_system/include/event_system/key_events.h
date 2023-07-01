#ifndef EVIE_EVENT_SYSTEM_KEY_EVENT_H_
#define EVIE_EVENT_SYSTEM_KEY_EVENT_H_

#include <sstream>

#include "events.h"

namespace evie {
class EVIE_API KeyEvent : public Event
{
public:
  [[nodiscard]] int GetKeyCode() const { return key_code_; };

  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Keyboard | EventCategoryBitmask::Input;
  }

  ~KeyEvent() noexcept override = default;

protected:
  explicit KeyEvent(int key_code) : key_code_(key_code) {}
  KeyEvent(const KeyEvent&) = default;
  KeyEvent(KeyEvent&&) = default;
  KeyEvent& operator=(const KeyEvent&) = default;
  KeyEvent& operator=(KeyEvent&&) = default;
  int key_code_;
};

class EVIE_API KeyPressedEvent final : public KeyEvent
{
public:
  static constexpr EventType type = EventType::KeyPressed;
  KeyPressedEvent(int key_code, int repeat_count) : KeyEvent(key_code), repeat_count_(repeat_count) {}
  KeyPressedEvent(const KeyPressedEvent&) = default;
  KeyPressedEvent(KeyPressedEvent&&) = default;
  KeyPressedEvent& operator=(const KeyPressedEvent&) = default;
  KeyPressedEvent& operator=(KeyPressedEvent&&) = default;
  ~KeyPressedEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return KeyPressedEvent::type; }

  [[nodiscard]] int GetRepeatCount() const { return repeat_count_; };

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "KeyPressedEvent: " << key_code_ << " (" << repeat_count_ << " repeats)";
    return stream.str();
  }

private:
  int repeat_count_;
};

class EVIE_API KeyReleasedEvent final : public KeyEvent
{
public:
  static constexpr EventType type = EventType::KeyReleased;

  explicit KeyReleasedEvent(int key_code) : KeyEvent(key_code) {}
  KeyReleasedEvent(const KeyReleasedEvent&) = default;
  KeyReleasedEvent(KeyReleasedEvent&&) = default;
  KeyReleasedEvent& operator=(const KeyReleasedEvent&) = default;
  KeyReleasedEvent& operator=(KeyReleasedEvent&&) = default;
  ~KeyReleasedEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return KeyReleasedEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "KeyReleasedEvent: " << key_code_ << "\n";
    return stream.str();
  }
};

}// namespace evie

#endif// !EVIE_EVENT_SYSTEM_KEY_EVENT_H_