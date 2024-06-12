#ifndef EVIE_WINDOW_H_
#define EVIE_WINDOW_H_

#include <memory>

#include <spdlog/common.h>

#include "evie/core.h"
#include "evie/error.h"
#include "evie/types.h"
#include "evie/window_events.h"
#include "window/event_manager.h"

namespace evie {

class EVIE_API IWindow
{
public:
  virtual ~IWindow() = default;
  [[nodiscard]] virtual Error Initialise(const WindowProperties& props) = 0;
  virtual void Destroy() = 0;
  virtual void PollEvents() = 0;
  virtual void SwapBuffers() = 0;
  [[nodiscard]] virtual Error RegisterEventManager(EventManager& event_manager) = 0;
  virtual void* GetNativeWindow() = 0;
  virtual void EnableCursor() = 0;
  virtual void DisableCursor() = 0;
  virtual const WindowProperties& GetWindowProperties() = 0;
  virtual float GetAspectRatio() = 0;
};


}// namespace evie

#endif// EVIE_WINDOW_H_