#ifndef EVIE_I_INPUT_MANAGER_H_
#define EVIE_I_INPUT_MANAGER_H_

#include "evie/events.h"
#include "evie/input.h"

namespace evie {
/**
 * @brief Interface for the InputManager so that platforms can make their own implementation.
 *
 */
class EVIE_API IInputManager
{
public:
  virtual ~IInputManager() = default;
  virtual void RegisterInput(const Event& event) = 0;
  [[nodiscard]] virtual bool IsKeyPressed(const KeyCode code) const = 0;
  [[nodiscard]] virtual bool IsMousePressed(const MouseCode code) const = 0;
};
}// namespace evie

#endif// ! EVIE_I_INPUT_MANAGER_H_