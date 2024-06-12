#ifndef EVIE_WINDOWS_H_
#define EVIE_WINDOWS_H_

#include "evie/error.h"
#include "evie/events.h"
#include "evie/window.h"
#include "window/event_manager.h"

#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include <evie/types.h>

namespace evie {

// We could have separate concrete classes for Windows, macOS and Linux but for each of these platforms we'll be using
// GLFW. When it comes to other platforms like android and ios we can make a new implementation.
// The name might be slightly confusing considering GLFW has a GLFWwindow type.
class EVIE_API GLFWWindow final : public IWindow
{
public:
  ~GLFWWindow() override = default;

  [[nodiscard]] Error Initialise(const WindowProperties& props) override;
  void Destroy() override;
  void PollEvents() override;
  void SwapBuffers() override;
  [[nodiscard]] Error RegisterEventManager(EventManager& event_manager) override;
  void* GetNativeWindow() override;
  EventManager* GetEventManager();
  void SetVSync(bool enabled);
  void DisableCursor() override;
  void EnableCursor() override;

  const WindowProperties& GetWindowProperties() override { return properties_; }
  void UpdateWindowDimensions(const WindowDimensions& dimensions) noexcept;
  float GetAspectRatio() override;

private:
  [[nodiscard]] Error DispatchEvent(std::unique_ptr<Event> event);
  void SetupInputCallbacks();
  GLFWwindow* window_{ nullptr };
  WindowProperties properties_;
  EventManager* event_manager_{ nullptr };
  bool vsync_enabled_{ false };
};

}// namespace evie

#endif// !EVIE_WINDOWS_H_