#include <doctest/doctest.h>

#include "evie/input.h"
#include "window/input_manager_impl.h"
#include "evie/key_events.h"
#include "evie/mouse_events.h"

TEST_CASE("InputManager IsKeyPressed Tests")
{
  evie::InputManager input_manager;
  SUBCASE("Check lowest key enum registers when pressed")
  {
    evie::KeyPressedEvent event(static_cast<uint16_t>(evie::KeyCode::Space), 0);
    input_manager.RegisterInput(event);
    REQUIRE(input_manager.IsKeyPressed(evie::KeyCode::Space));
    SUBCASE("Check lowest key enum registers when released")
    {
      evie::KeyReleasedEvent released_event(static_cast<uint16_t>(evie::KeyCode::Space));
      input_manager.RegisterInput(released_event);
      REQUIRE(!input_manager.IsKeyPressed(evie::KeyCode::Space));
    }
  }
  SUBCASE("Check highest key enum registers when pressed")
  {
    evie::KeyPressedEvent event(static_cast<uint16_t>(evie::KeyCode::Menu), 0);
    input_manager.RegisterInput(event);
    REQUIRE(input_manager.IsKeyPressed(evie::KeyCode::Menu));
    SUBCASE("Check highest key enum registers when released")
    {
      evie::KeyReleasedEvent released_event(static_cast<uint16_t>(evie::KeyCode::Menu));
      input_manager.RegisterInput(released_event);
      REQUIRE(!input_manager.IsKeyPressed(evie::KeyCode::Menu));
    }
  }
  SUBCASE("Check KeyCode::NumberOfCodes is correct")
  {
    REQUIRE(static_cast<uint16_t>(evie::KeyCode::Menu) + 1 == static_cast<uint16_t>(evie::KeyCode::NumberOfCodes));
  }
}

TEST_CASE("InputManager IsMousePressed Tests")
{
  evie::InputManager input_manager;
  SUBCASE("Check lowest mouse button enum registers when pressed")
  {
    evie::MousePressedEvent event(static_cast<uint16_t>(evie::MouseCode::Button1));
    input_manager.RegisterInput(event);
    REQUIRE(input_manager.IsMousePressed(evie::MouseCode::Button1));
    SUBCASE("Check lowest mouse button enum registers when released")
    {
      evie::MouseReleasedEvent released_event(static_cast<uint16_t>(evie::MouseCode::Button1));
      input_manager.RegisterInput(released_event);
      REQUIRE(!input_manager.IsMousePressed(evie::MouseCode::Button1));
    }
  }
  SUBCASE("Check highest mouse button enum registers when pressed")
  {
    evie::MousePressedEvent event(static_cast<uint16_t>(evie::MouseCode::Button8));
    input_manager.RegisterInput(event);
    REQUIRE(input_manager.IsMousePressed(evie::MouseCode::Button8));
    SUBCASE("Check highest mouse button enum registers when released")
    {
      evie::MouseReleasedEvent released_event(static_cast<uint16_t>(evie::MouseCode::Button8));
      input_manager.RegisterInput(released_event);
      REQUIRE(!input_manager.IsMousePressed(evie::MouseCode::Button8));
    }
  }
  SUBCASE("Check MouseCode::NumberOfCodes is correct")
  {
    REQUIRE(static_cast<uint16_t>(evie::MouseCode::Button8) + 1 == static_cast<uint16_t>(evie::MouseCode::NumberOfCodes));
  }
  SUBCASE("Check MouseCode enum equalities don't change")
  {
    REQUIRE(evie::MouseCode::Button1 == evie::MouseCode::ButtonLeft);
    REQUIRE(evie::MouseCode::Button2 == evie::MouseCode::ButtonRight);
    REQUIRE(evie::MouseCode::Button3 == evie::MouseCode::ButtonMiddle);
    REQUIRE(evie::MouseCode::Button8 == evie::MouseCode::ButtonLast);
  }
}