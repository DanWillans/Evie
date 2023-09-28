#ifndef EVIE_INPUT_MANAGER_H_
#define EVIE_INPUT_MANAGER_H_

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
  virtual bool IsKeyPressed(const EvieKey keycode) const = 0;
};
}// namespace evie

#endif// ! EVIE_INPUT_MANAGER_H_