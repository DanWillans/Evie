#include <doctest/doctest.h>
#include <memory>

#include "evie/events.h"
#include "window/event_manager.h"
#include "window/key_events.h"
#include "window/layer_queue.h"
#include "window/mouse_events.h"
#include "window/window_events.h"

using ECB = evie::EventCategoryBitmask;

namespace {

class TestLayer final : public evie::Layer
{
public:
  void OnUpdate() override {}
  void OnEvent(evie::Event& event) override
  {
    if (event.GetEventType() == evie::EventType::KeyPressed) {
      auto* key_event = static_cast<evie::KeyPressedEvent*>(&event);
      [[maybe_unused]] auto code = key_event->GetKeyCode();
      event_count++;
    }
  }
  ~TestLayer() override = default;
  int event_count{ 0 };
};

class TestLayer2 final : public evie::Layer
{
public:
  void OnUpdate() override {}
  void OnEvent(evie::Event& event) override
  {
    if (event.GetEventType() == evie::EventType::MouseButtonPressed) {
      auto* key_event = static_cast<evie::MousePressedEvent*>(&event);
      [[maybe_unused]] auto code = key_event->GetMouseButton();
      (void)code;
      event_count++;
    }
  }
  ~TestLayer2() override = default;
  int event_count{ 0 };
};
}// namespace

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
  evie::MousePosition position{ 100.0, 200.0 };
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

TEST_CASE("EventManager Tests")
{
  TestLayer layer1;
  TestLayer2 layer2;
  evie::LayerQueue layer_queue;
  layer_queue.PushBack(layer1);
  layer_queue.PushBack(layer2);
  evie::EventManager event_manager(layer_queue);
  SUBCASE("Check subscribing to event type works")
  {
    evie::WindowCloseEvent window_close_event;
    auto callback = [&](const evie::Event& event) {
      REQUIRE(event.GetEventType() == window_close_event.GetEventType());
      return true;
    };
    event_manager.SubscribeToEventType(evie::EventType::WindowClose, callback);
    event_manager.OnEvent(window_close_event);
    REQUIRE(layer1.event_count == 0);
    REQUIRE(layer2.event_count == 0);
  }
  SUBCASE("Check layer1 receives event but not layer2")
  {
    evie::KeyPressedEvent key_pressed_event(20, 2);
    event_manager.OnEvent(key_pressed_event);
    REQUIRE(layer1.event_count == 1);
    REQUIRE(layer2.event_count == 0);
    SUBCASE("Check layer2 now receives event but not layer1")
    {
      evie::MousePressedEvent mouse_pressed_event(evie::MouseButton::Left);
      event_manager.OnEvent(mouse_pressed_event);
      REQUIRE(layer1.event_count == 1);
      REQUIRE(layer2.event_count == 1);
    }
  }
  SUBCASE("Check layer2 receives event but not layer1")
  {
    evie::MousePressedEvent mouse_pressed_event(evie::MouseButton::Left);
    event_manager.OnEvent(mouse_pressed_event);
    REQUIRE(layer1.event_count == 0);
    REQUIRE(layer2.event_count == 1);
    SUBCASE("Check layer1 now receives event but layer2")
    {
      evie::KeyPressedEvent key_pressed_event(20, 2);
      event_manager.OnEvent(key_pressed_event);
      REQUIRE(layer1.event_count == 1);
      REQUIRE(layer2.event_count == 1);
    }
  }
  SUBCASE("Check that two layers wanting the same event don't both receive it")
  {
    TestLayer2 layer3;
    layer_queue.PushBack(layer3);
    evie::MousePressedEvent mouse_pressed_event(evie::MouseButton::Left);
    event_manager.OnEvent(mouse_pressed_event);
    REQUIRE(layer1.event_count == 0);
    REQUIRE(layer2.event_count == 0);
    REQUIRE(layer3.event_count == 1);
  }
}