#include "evie/events.h"
#include "window/glfw_input_manager.h"
#include "window/key_events.h"
#include <imgui.h>


namespace evie {
void GLFWInputManager::RegisterInput(const Event& event)
{
  // Check if event is a keyboard event
  if (event.IsInCategory(EventCategoryBitmask::Keyboard)) {
    const KeyEvent* key_event = static_cast<const KeyEvent*>(&event);
    if (key_event->GetEventType() == EventType::KeyPressed) {
      key_state_map_[key_event->GetKeyCode()] = 1;
    } else if (key_event->GetEventType() == EventType::KeyReleased) {
      key_state_map_[key_event->GetKeyCode()] = 0;
    }
  }
}

bool GLFWInputManager::IsKeyPressed(const EvieKey keycode) const
{
  return static_cast<bool>(key_state_map_[static_cast<int>(keycode)]);
}

}// namespace evie