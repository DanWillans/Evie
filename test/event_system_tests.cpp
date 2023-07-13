#include <doctest/doctest.h>

#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "event_system/window_events.h"

using ECB = evie::EventCategoryBitmask;

TEST_CASE("KeyPressedEvent tests")
{
  int key_code = 1;
  int repeat_count = 2;
  evie::KeyPressedEvent event(key_code, repeat_count);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::KeyPressed); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Keyboard));
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.GetCategoryFlags() == (ECB::Keyboard | ECB::Input));
  }
  SUBCASE("Check return key code is correct") { REQUIRE(event.GetKeyCode() == key_code); }
  SUBCASE("Check return repeat count is correct") { REQUIRE(event.GetRepeatCount() == repeat_count); }
}

TEST_CASE("KeyReleasedEvent tests")
{
  int key_code = 1;
  evie::KeyReleasedEvent event(key_code);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::KeyReleased); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Keyboard));
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.GetCategoryFlags() == (ECB::Keyboard | ECB::Input));
  }
  SUBCASE("Check return key code is correct") { REQUIRE(event.GetKeyCode() == key_code); }
}

TEST_CASE("MousePressedEvent Tests")
{
  evie::MouseButton button = evie::MouseButton::Left;
  evie::MousePressedEvent event(button);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::MouseButtonPressed); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::MouseButton));
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.IsInCategory(ECB::Mouse));
    REQUIRE(event.GetCategoryFlags() == (ECB::Mouse | ECB::MouseButton | ECB::Input));
  }
  SUBCASE("Check mouse button return is correct") { REQUIRE(event.GetMouseButton() == button); }
}

TEST_CASE("MouseReleasedEvent Tests")
{
  evie::MouseButton button = evie::MouseButton::Left;
  evie::MouseReleasedEvent event(button);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::MouseButtonReleased); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::MouseButton));
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.IsInCategory(ECB::Mouse));
    REQUIRE(event.GetCategoryFlags() == (ECB::Mouse | ECB::MouseButton | ECB::Input));
  }
  SUBCASE("Check mouse button return is correct") { REQUIRE(event.GetMouseButton() == button); }
}

TEST_CASE("MouseScrolledEvent Tests")
{
  evie::MouseScrollOffset offset{ 100.0, 200.0 };
  evie::MouseScrolledEvent event(offset);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::MouseScrolled); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.IsInCategory(ECB::Mouse));
    REQUIRE(event.GetCategoryFlags() == (ECB::Mouse | ECB::Input));
  }
  SUBCASE("Check mouse scroll direction return is correct")
  {
    auto ret_offset = event.GetScrollOffset();
    REQUIRE(ret_offset.x_offset == offset.x_offset);
    REQUIRE(ret_offset.y_offset == offset.y_offset);
  }
}

TEST_CASE("MouseMovementEvent Tests")
{
  evie::MousePosition position{ 100.0F, 200.0F };
  evie::MouseMovementEvent event(position);
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::MouseMoved); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Input));
    REQUIRE(event.IsInCategory(ECB::Mouse));
    REQUIRE(event.GetCategoryFlags() == (ECB::Mouse | ECB::Input));
  }
  SUBCASE("Check mouse position return is correct")
  {
    evie::MousePosition ret = event.GetMousePosition();
    REQUIRE(ret.x == position.x);
    REQUIRE(ret.y == position.y);
  }
}

TEST_CASE("WindowCloseEvent Tests")
{
  evie::WindowCloseEvent event;
  SUBCASE("Check EventType is correct") { REQUIRE(event.GetEventType() == evie::EventType::WindowClose); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Application));
    REQUIRE(event.GetCategoryFlags() == (ECB::Application));
  }
}

TEST_CASE("WindowFocusEvent Tests")
{
  evie::WindowFocusEvent event;
  SUBCASE("Check EventType is correct ") { REQUIRE(event.GetEventType() == evie::EventType::WindowFocus); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Application));
    REQUIRE(event.GetCategoryFlags() == (ECB::Application));
  }
}

TEST_CASE("WindowLostFocusEvent Tests")
{
  evie::WindowLostFocusEvent event;
  SUBCASE("Check EventType is correct ") { REQUIRE(event.GetEventType() == evie::EventType::WindowLostFocus); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Application));
    REQUIRE(event.GetCategoryFlags() == (ECB::Application));
  }
}

TEST_CASE("WindowResizeEvent Tests")
{
  evie::WindowDimensions dims{ 100, 200 };
  evie::WindowResizeEvent event(dims);
  SUBCASE("Check EventType is correct ") { REQUIRE(event.GetEventType() == evie::EventType::WindowResize); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Application));
    REQUIRE(event.GetCategoryFlags() == (ECB::Application));
  }
  SUBCASE("Check returned dimensions are correct")
  {
    auto ret = event.GetWindowDimensions();
    REQUIRE(ret.height == dims.height);
    REQUIRE(ret.width == dims.width);
  }
}

TEST_CASE("WindowMovedEvent Tests")
{
  evie::WindowPosition pos{ 100, 200 };
  evie::WindowMovedEvent event(pos);
  SUBCASE("Check EventType is correct ") { REQUIRE(event.GetEventType() == evie::EventType::WindowMoved); }
  SUBCASE("Check EventCategory is correct")
  {
    REQUIRE(event.IsInCategory(ECB::Application));
    REQUIRE(event.GetCategoryFlags() == (ECB::Application));
  }
  SUBCASE("Check returned dimensions are correct")
  {
    auto ret = event.GetWindowPosition();
    REQUIRE(ret.x == pos.x);
    REQUIRE(ret.y == pos.y);
  }
}