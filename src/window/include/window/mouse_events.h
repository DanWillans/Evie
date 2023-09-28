#ifndef EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_
#define EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_

#include <sstream>

#include "evie/core.h"
#include "evie/events.h"
#include "evie/logging.h"


namespace evie {
struct EVIE_API MousePosition
{
  double x{ 0 };
  double y{ 0 };
};

struct EVIE_API MouseScrollOffset
{
  double x_offset{ 0 };
  double y_offset{ 0 };
};

class EVIE_API MouseButtonEvent : public Event
{
public:
  explicit MouseButtonEvent(int button) : button_(button) {}
  MouseButtonEvent(const MouseButtonEvent&) = default;
  MouseButtonEvent(MouseButtonEvent&&) = default;
  MouseButtonEvent& operator=(const MouseButtonEvent&) = default;
  MouseButtonEvent& operator=(MouseButtonEvent&&) = default;
  ~MouseButtonEvent() noexcept override = default;
  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Input | EventCategoryBitmask::Mouse | EventCategoryBitmask::MouseButton;
  }

  [[nodiscard]] int GetMouseButton() const { return button_; }

protected:
  int button_ = -1;
};

class EVIE_API MousePressedEvent final : public MouseButtonEvent
{
public:
  static constexpr EventType type = EventType::MouseButtonPressed;

  using MouseButtonEvent::MouseButtonEvent;
  MousePressedEvent(const MousePressedEvent&) = default;
  MousePressedEvent(MousePressedEvent&&) = default;
  MousePressedEvent& operator=(const MousePressedEvent&) = default;
  MousePressedEvent& operator=(MousePressedEvent&&) = default;
  ~MousePressedEvent() noexcept override = default;


  [[nodiscard]] constexpr EventType GetEventType() const override { return MousePressedEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MousePressedEvent: " << static_cast<int>(button_) << "\n";
    return stream.str();
  }
};

class EVIE_API MouseReleasedEvent final : public MouseButtonEvent
{
public:
  static constexpr EventType type = EventType::MouseButtonReleased;

  using MouseButtonEvent::MouseButtonEvent;
  MouseReleasedEvent(const MouseReleasedEvent&) = default;
  MouseReleasedEvent(MouseReleasedEvent&&) = default;
  MouseReleasedEvent& operator=(const MouseReleasedEvent&) = default;
  MouseReleasedEvent& operator=(MouseReleasedEvent&&) = default;
  ~MouseReleasedEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return MouseReleasedEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MouseReleasedEvent: " << static_cast<int>(button_) << "\n";
    return stream.str();
  }
};

class EVIE_API MouseMovementEvent final : public Event
{
public:
  static constexpr EventType type = EventType::MouseMoved;

  explicit MouseMovementEvent(const MousePosition& position) : position_(position) {}
  MouseMovementEvent(const MouseMovementEvent&) = default;
  MouseMovementEvent(MouseMovementEvent&&) = default;
  MouseMovementEvent& operator=(const MouseMovementEvent&) = default;
  MouseMovementEvent& operator=(MouseMovementEvent&&) = default;
  ~MouseMovementEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return MouseMovementEvent::type; }

  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Input | EventCategoryBitmask::Mouse;
  }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MouseMovementEvent: x - " << position_.x << " y - " << position_.y << "\n";
    return stream.str();
  }

  [[nodiscard]] const MousePosition& GetMousePosition() { return position_; }


private:
  MousePosition position_;
};

class EVIE_API MouseScrolledEvent final : public Event
{
public:
  static constexpr EventType type = EventType::MouseScrolled;

  explicit MouseScrolledEvent(const MouseScrollOffset& scroll_offset) : scroll_offset_(scroll_offset) {}
  MouseScrolledEvent(const MouseScrolledEvent&) = default;
  MouseScrolledEvent(MouseScrolledEvent&&) = default;
  MouseScrolledEvent& operator=(const MouseScrolledEvent&) = default;
  MouseScrolledEvent& operator=(MouseScrolledEvent&&) = default;
  ~MouseScrolledEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return MouseScrolledEvent::type; }

  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Input | EventCategoryBitmask::Mouse;
  }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MouseScolledEvent: x_offset - " << scroll_offset_.x_offset << " y_offset - " << scroll_offset_.y_offset
           << "\n";
    return stream.str();
  }

  [[nodiscard]] MouseScrollOffset GetScrollOffset() { return scroll_offset_; }

private:
  MouseScrollOffset scroll_offset_{ 0, 0 };
};
}// namespace evie

#endif// EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_