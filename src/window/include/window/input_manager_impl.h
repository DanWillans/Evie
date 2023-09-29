#ifndef EVIE_INPUT_MANAGER_H_
#define EVIE_INPUT_MANAGER_H_

#include "evie/events.h"
#include "evie/input.h"
#include "evie/input_manager.h"

namespace evie {
class EVIE_API InputManager final : public IInputManager
{
public:
  ~InputManager() override = default;
  void RegisterInput(const Event& event) override;
  bool IsKeyPressed(const KeyCode code) const override;
  bool IsMousePressed(const MouseCode code) const override;

private:
  void UpdateMouseState(const Event& event);
  void UpdateKeyState(const Event& event);
  uint8_t key_state_map_[static_cast<uint16_t>(KeyCode::NumberOfCodes)] = { 0 };
  uint8_t mouse_state_map_[static_cast<uint16_t>(MouseCode::NumberOfCodes)] = { 0 };
};
}// namespace evie

#endif// !EVIE_INPUT_MANAGER_H_