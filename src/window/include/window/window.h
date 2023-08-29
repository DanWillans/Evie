#ifndef EVIE_WINDOW_H_
#define EVIE_WINDOW_H_

#include <memory>
#include <spdlog/common.h>

#include "window/event_manager.h"
#include "window/window_events.h"
#include "evie/core.h"
#include "evie/error.h"
namespace evie {
struct WindowProperties
{
  WindowDimensions dimensions;
  std::string name;
};
// Currently this class handles initialising glfw and creating a window.
// It might make sure sense to just handle window creation and something else
// handle the init.
class EVIE_API Window
{
public:
  explicit Window(const WindowProperties& window_props);
  ~Window();
  // For now the Window class shouldn't be allowed to be copied or moved.
  Window(const Window&) = delete;
  Window(Window&&) = delete;
  Window& operator=(const Window&) = delete;
  Window& operator=(Window&&) = delete;

  Error Initialise();
  void Update();
  Error RegisterEventManager(EventManager& event_manager);
  EventManager* GetEventManager();
  void SetVSyncFlag(bool enabled);

private:
  // Impl at the minute to avoid leaking the implementation to the user
  class Impl;
  Impl* impl_;
};
}// namespace evie

#endif// EVIE_WINDOW_H_