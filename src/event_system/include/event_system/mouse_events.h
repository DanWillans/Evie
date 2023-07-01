#ifndef EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_
#define EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_

#include <sstream>

#include "events.h"
#include "evie/core.h"
#include "evie/logging.h"


namespace evie {
enum class MouseButton { None = 0, Left = 1, Right = 2, Middle = 3 };

struct MousePosition
{
  float x{ 0 };
  float y{ 0 };
};

class EVIE_API MousePressedEvent final : public Event
{
public:
  static constexpr EventType type = EventType::MouseButtonPressed;

  explicit MousePressedEvent(MouseButton button) : button_(button) {}
  MousePressedEvent(const MousePressedEvent&) = default;
  MousePressedEvent(MousePressedEvent&&) = default;
  MousePressedEvent& operator=(const MousePressedEvent&) = default;
  MousePressedEvent& operator=(MousePressedEvent&&) = default;
  ~MousePressedEvent() noexcept override = default;

  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Input | EventCategoryBitmask::Mouse | EventCategoryBitmask::MouseButton;
  }

  [[nodiscard]] constexpr EventType GetEventType() const override { return MousePressedEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MousePressedEvent: " << static_cast<int>(button_) << "\n";
    return stream.str();
  }

  [[nodiscard]] MouseButton GetMouseButton() const { return button_; }

private:
  MouseButton button_ = MouseButton::None;
};

class EVIE_API MouseReleasedEvent final : public Event
{
public:
  static constexpr EventType type = EventType::MouseButtonReleased;

  explicit MouseReleasedEvent(MouseButton button) : button_(button) {}
  MouseReleasedEvent(const MouseReleasedEvent&) = default;
  MouseReleasedEvent(MouseReleasedEvent&&) = default;
  MouseReleasedEvent& operator=(const MouseReleasedEvent&) = default;
  MouseReleasedEvent& operator=(MouseReleasedEvent&&) = default;
  ~MouseReleasedEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return MouseReleasedEvent::type; }

  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {

    return EventCategoryBitmask::Input | EventCategoryBitmask::Mouse | EventCategoryBitmask::MouseButton;
  }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "MouseReleasedEvent: " << static_cast<int>(button_) << "\n";
    return stream.str();
  }

  [[nodiscard]] MouseButton GetMouseButton() const { return button_; }

private:
  MouseButton button_ = MouseButton::None;
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

  [[nodiscard]] const MousePosition& GetMousePosition() const { return position_; }


private:
  MousePosition position_;
};

class EVIE_API MouseScrolledEvent final : public Event
{
public:
  static constexpr EventType type = EventType::MouseScrolled;

  explicit MouseScrolledEvent(int scroll_direction) : scroll_direction_(scroll_direction) {}
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
    stream << "MouseScolledEvent: direction - " << scroll_direction_ << "\n";
    return stream.str();
  }

  [[nodiscard]] int GetScrollDirection() const { return scroll_direction_; }

private:
  int scroll_direction_{ 0 };
};
}// namespace evie

#endif// EVIE_EVENT_SYSTEM_MOUSE_EVENTS_H_