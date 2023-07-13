#ifndef EVIE_WINDOW_H_
#define EVIE_WINDOW_H_

#include <memory>
#include <spdlog/common.h>

#include "event_system/event_manager.h"
#include "evie/core.h"
#include "evie/error.h"


namespace evie {
// Currently this class handles initialising glfw and creating a window.
// It might make sure sense to just handle window creation and something else
// handle the init.
class EVIE_API Window
{
public:
  explicit Window(const char* window_name);
  ~Window();
  // For now the Window class shouldn't be allowed to be copied or moved.
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;

  Error Initialise();
  void Update();
  Error RegisterEventListener(IEventListener* event_listener);
  IEventListener* GetEventListener();

private:
  // Impl at the minute to avoid leaking the implementation to the user
  class Impl;
  Impl* impl_;
};
}// namespace evie

#endif// EVIE_WINDOW_H_