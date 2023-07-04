#ifndef EVIE_EVENT_SYSTEM_WINDOW_EVENTS_H_
#define EVIE_EVENT_SYSTEM_WINDOW_EVENTS_H_

#include <sstream>

#include "events.h"

namespace evie {
struct WindowDimensions
{
  uint16_t width{ 0 };
  uint16_t height{ 0 };
};

struct WindowPosition
{
  uint16_t x{ 0 };
  uint16_t y{ 0 };
};

class EVIE_API WindowEvent : public Event
{
public:
  [[nodiscard]] constexpr EventCategoryBitmask GetCategoryFlags() const override
  {
    return EventCategoryBitmask::Application;
  }

  ~WindowEvent() noexcept override = default;

protected:
  explicit WindowEvent() = default;
  WindowEvent(const WindowEvent&) = default;
  WindowEvent(WindowEvent&&) = default;
  WindowEvent& operator=(const WindowEvent&) = default;
  WindowEvent& operator=(WindowEvent&&) = default;
};

class EVIE_API WindowCloseEvent final : public WindowEvent
{
public:
  static constexpr EventType type = EventType::WindowClose;
  WindowCloseEvent() = default;
  WindowCloseEvent(const WindowCloseEvent&) = default;
  WindowCloseEvent(WindowCloseEvent&&) = default;
  WindowCloseEvent& operator=(const WindowCloseEvent&) = default;
  WindowCloseEvent& operator=(WindowCloseEvent&&) = default;
  ~WindowCloseEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return WindowCloseEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "WindowCloseEvent\n";
    return stream.str();
  }
};

class EVIE_API WindowFocusEvent final : public WindowEvent
{
public:
  static constexpr EventType type = EventType::WindowFocus;
  WindowFocusEvent() = default;
  WindowFocusEvent(const WindowFocusEvent&) = default;
  WindowFocusEvent(WindowFocusEvent&&) = default;
  WindowFocusEvent& operator=(const WindowFocusEvent&) = default;
  WindowFocusEvent& operator=(WindowFocusEvent&&) = default;
  ~WindowFocusEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return WindowFocusEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "WindowCloseEvent\n";
    return stream.str();
  }
};

class EVIE_API WindowLostFocusEvent final : public WindowEvent
{
public:
  static constexpr EventType type = EventType::WindowLostFocus;
  WindowLostFocusEvent() = default;
  WindowLostFocusEvent(const WindowLostFocusEvent&) = default;
  WindowLostFocusEvent(WindowLostFocusEvent&&) = default;
  WindowLostFocusEvent& operator=(const WindowLostFocusEvent&) = default;
  WindowLostFocusEvent& operator=(WindowLostFocusEvent&&) = default;
  ~WindowLostFocusEvent() noexcept override = default;

  [[nodiscard]] constexpr EventType GetEventType() const override { return WindowLostFocusEvent::type; }

  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "WindowCloseEvent\n";
    return stream.str();
  }
};

class EVIE_API WindowResizeEvent final : public WindowEvent
{
public:
  static constexpr EventType type = EventType::WindowResize;
  explicit WindowResizeEvent(const WindowDimensions& dimensions) : dimensions_(dimensions) {}
  WindowResizeEvent(const WindowResizeEvent&) = default;
  WindowResizeEvent(WindowResizeEvent&&) = default;
  WindowResizeEvent& operator=(const WindowResizeEvent&) = default;
  WindowResizeEvent& operator=(WindowResizeEvent&&) = default;
  ~WindowResizeEvent() noexcept override = default;
  [[nodiscard]] constexpr EventType GetEventType() const override { return WindowResizeEvent::type; }
  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "WindowResizeEvent: Width - " << dimensions_.width << " Height - " << dimensions_.height << "\n";
    return stream.str();
  }
  [[nodiscard]] const WindowDimensions& GetWindowDimensions() const { return dimensions_; }

private:
  WindowDimensions dimensions_;
};

class EVIE_API WindowMovedEvent final : public WindowEvent
{
public:
  static constexpr EventType type = EventType::WindowMoved;
  explicit WindowMovedEvent(const WindowPosition& position) : position_(position) {}
  WindowMovedEvent(const WindowMovedEvent&) = default;
  WindowMovedEvent(WindowMovedEvent&&) = default;
  WindowMovedEvent& operator=(const WindowMovedEvent&) = default;
  WindowMovedEvent& operator=(WindowMovedEvent&&) = default;
  ~WindowMovedEvent() noexcept override = default;
  [[nodiscard]] constexpr EventType GetEventType() const override { return WindowMovedEvent::type; }
  [[nodiscard]] std::string ToString() const override
  {
    std::stringstream stream;
    stream << "WindowMovedEvent: Position X - " << position_.x << " Position Y - " << position_.y << "\n";
    return stream.str();
  }

  [[nodiscard]] const WindowPosition& GetWindowPosition() const { return position_; }

private:
  WindowPosition position_;
};


}// namespace evie

#endif// EVIE_EVENT_SYSTEM_WINDOW_EVENTS_H_