#ifndef EVIE_GLFW_INPUT_MANAGER_H_
#define EVIE_GLFW_INPUT_MANAGER_H_

#include "evie/events.h"
#include "evie/input.h"
#include "evie/input_manager.h"

namespace evie {
class EVIE_API GLFWInputManager final : public IInputManager
{
public:
  ~GLFWInputManager() override = default;
  void RegisterInput(const Event& event) override;
  bool IsKeyPressed(const EvieKey keycode) const override;

private:
  uint8_t key_state_map_[static_cast<unsigned long>(EvieKey::NumberOfCodes)] = { 0 };
};
}// namespace evie

#endif// !EVIE_INPUT_MANAGER_H_