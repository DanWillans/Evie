#ifndef EVIE_WINDOWS_H_
#define EVIE_WINDOWS_H_

#include "evie/error.h"
#include "evie/events.h"
#include "window/event_manager.h"
#include "window/window.h"

#include "glfw/glfw3.h"

namespace evie {

// We could have separate concrete classes for Windows, macOS and Linux but for each of these platforms we'll be using
// GLFW. When it comes to other platforms like android and ios we can make a new implementation.
// The name might be slightly confusing considering GLFW has a GLFWwindow type.
class EVIE_API GLFWWindow final : IWindow
{
public:
  ~GLFWWindow() override = default;
  // For now the Window class shouldn't be allowed to be copied or moved.

  Error Initialise(const WindowProperties& props) override;
  void Destroy() override;
  void PollEvents() override;
  void SwapBuffers() override;
  Error RegisterEventManager(EventManager& event_manager) override;
  EventManager* GetEventManager();
  void* GetNativeWindow() override;
  void SetVSync(bool enabled);

private:
  Error DispatchEvent(std::unique_ptr<Event> event);
  void SetupInputCallbacks();
  GLFWwindow* window_{ nullptr };
  WindowProperties properties_;
  EventManager* event_manager_{ nullptr };
  bool vsync_enabled_{ false };
};

}// namespace evie

#endif// !EVIE_WINDOWS_H_