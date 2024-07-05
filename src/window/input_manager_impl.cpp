#include "window/input_manager_impl.h"
#include "evie/events.h"
#include "evie/key_events.h"
#include "evie/mouse_events.h"

namespace evie {
void InputManager::RegisterInput(const Event& event)
{
  if (event.IsInCategory(EventCategoryBitmask::Keyboard)) {
    UpdateMouseState(event);
  } else if (event.IsInCategory(EventCategoryBitmask::MouseButton)) {
    UpdateKeyState(event);
  }
}

bool InputManager::IsKeyPressed(const KeyCode code) const
{
  return static_cast<bool>(key_state_map_[static_cast<int>(code)]);
}

bool InputManager::IsMousePressed(const MouseCode code) const
{
  return static_cast<bool>(mouse_state_map_[static_cast<int>(code)]);
}

void InputManager::UpdateMouseState(const Event& event)
{
  const KeyEvent* key_event = static_cast<const KeyEvent*>(&event);
  EventType event_type = key_event->GetEventType();
  if (event_type == EventType::KeyPressed) {
    key_state_map_[key_event->GetKeyCode()] = 1;
  } else if (event_type == EventType::KeyReleased) {
    key_state_map_[key_event->GetKeyCode()] = 0;
  }
}

void InputManager::UpdateKeyState(const Event& event)
{
  const MouseButtonEvent* mouse_event = static_cast<const MouseButtonEvent*>(&event);
  EventType event_type = mouse_event->GetEventType();
  int button = mouse_event->GetMouseButton();
  if (event_type == EventType::MouseButtonPressed) {
    if (button >= MouseCode::Button1 && button <= MouseCode::NumberOfCodes) {
      mouse_state_map_[button] = 1;
    } else {
      EV_ERROR("Unrecognised mouse button with value - {}. Please report to Evie developers.", button);
    }
  } else if (event_type == EventType::MouseButtonReleased) {
    if (button >= MouseCode::Button1 && button <= MouseCode::NumberOfCodes) {
      mouse_state_map_[button] = 0;
    } else {
      EV_ERROR("Unrecognised mouse button with value - {}. Please report to Evie developers.", button);
    }
  }
}

}// namespace evie