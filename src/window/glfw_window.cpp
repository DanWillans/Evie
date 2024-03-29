#include "window/glfw_window.h"
#include "evie/error.h"
#include "evie/logging.h"
#include "window/event_manager.h"
#include "window/key_events.h"
#include "window/mouse_events.h"
#include "window/window.h"
#include "window/window_events.h"


#ifdef EVIE_PLATFORM_WINDOWS
#include <Windows.h>
#endif
// Need this space between glfw and windows otherwise there is a macro redefinition error

#include "GLFW/glfw3.h"

namespace evie {
namespace {
  static void glfw_error_callback(int error, const char* description)
  {
    EV_ERROR("GLFW Error {}: {}", error, description);
  }

  EventManager* GetEventManager(GLFWwindow* window)
  {
    auto* user_window = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (user_window != nullptr) {
      auto* event_manager = user_window->GetEventManager();
      if (event_manager != nullptr) {
        return event_manager;
      } else {
        EV_WARN("No valid event_manager, dropping event");
      }
    } else {
      EV_WARN("Invalid user window pointer");
    }
    return nullptr;
  }

  template<typename T, typename... Args> void DispatchEvent(EventManager* listener, Args... args)
  {
    T event(args...);
    listener->OnEvent(event);
  }

  void KeyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      // I'm not sure I like how we handle repeat keys in this function but if it becomes a problem then we'll handle
      // it in the future. Also if this ever becomes multi threaded then it needs fixing.
      static int repeat_count{ 0 };
      switch (action) {
      case GLFW_PRESS:
        repeat_count = 0;
        DispatchEvent<KeyPressedEvent>(event_manager, key, 0);
        break;
      case GLFW_RELEASE:
        repeat_count = 0;
        DispatchEvent<KeyReleasedEvent>(event_manager, key);
        break;
      case GLFW_REPEAT: {
        repeat_count++;
        DispatchEvent<KeyPressedEvent>(event_manager, key, repeat_count);
        break;
      }
      default:
        EV_WARN("Unsupported key action {}", action);
      }
    }
  }

  void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      DispatchEvent<MouseMovementEvent>(event_manager, MousePosition{ xpos, ypos });
    }
  }

  void MouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      switch (action) {
      case GLFW_PRESS:
        DispatchEvent<MousePressedEvent>(event_manager, button);
        break;
      case GLFW_RELEASE:
        DispatchEvent<MouseReleasedEvent>(event_manager, button);
        break;
      default:
        EV_WARN("Unsupported mouse action {}", action);
      }
    }
  }

  void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      DispatchEvent<MouseScrolledEvent>(event_manager, MouseScrollOffset{ xoffset, yoffset });
    }
  }

  void WindowFocusCallback(GLFWwindow* window, int focused)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      if (focused == 1) {
        DispatchEvent<WindowFocusEvent>(event_manager);
      } else {
        DispatchEvent<WindowLostFocusEvent>(event_manager);
      }
    }
  }

  void WindowSizeCallback(GLFWwindow* window, int width, int height)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      DispatchEvent<WindowResizeEvent>(event_manager, WindowDimensions{ width, height });
    }
  }

  void WindowCloseCallback(GLFWwindow* window)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      DispatchEvent<WindowCloseEvent>(event_manager);
    }
  }

  void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
  {
    auto* event_manager = GetEventManager(window);
    if (event_manager != nullptr) {
      DispatchEvent<WindowMovedEvent>(event_manager, WindowPosition{ xpos, ypos });
    }
  }
}// namespace

Error GLFWWindow::Initialise(const WindowProperties& props)
{
  properties_ = props;
  glfwSetErrorCallback(glfw_error_callback);
  // We may want more than one window so we should fix this if we do. Maybe use a static?
  if (glfwInit() == GLFW_FALSE) {
    return Error("glfw failed to initialise");
  }
  window_ = glfwCreateWindow(
    properties_.dimensions.width, properties_.dimensions.height, properties_.name.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    return Error("glfw failed to create window");
  }
  glfwMakeContextCurrent(window_);
  // If we use glad we should init it here.
  SetVSync(true);
  // Setup the user window pointer
  glfwSetWindowUserPointer(window_, static_cast<void*>(this));
  SetupInputCallbacks();
  return Error::OK();
}

void GLFWWindow::SetupInputCallbacks()
{
  glfwSetKeyCallback(window_, KeyCallback);
  glfwSetCursorPosCallback(window_, CursorPositionCallback);
  glfwSetMouseButtonCallback(window_, MouseButtonCallback);
  glfwSetScrollCallback(window_, MouseScrollCallback);
  glfwSetWindowFocusCallback(window_, WindowFocusCallback);
  glfwSetWindowSizeCallback(window_, WindowSizeCallback);
  glfwSetWindowCloseCallback(window_, WindowCloseCallback);
  glfwSetWindowPosCallback(window_, WindowPositionCallback);
}

void GLFWWindow::PollEvents() { glfwPollEvents(); }

void GLFWWindow::SwapBuffers() { glfwSwapBuffers(window_); }

Error GLFWWindow::RegisterEventManager(EventManager& event_manager)
{
  if (event_manager_ == nullptr) {
    event_manager_ = &event_manager;
  } else {
    return Error("EventManager already exists. Only one event manager can be registered for now");
  }
  return Error::OK();
};

void GLFWWindow::SetVSync(bool enabled)
{
  if (enabled) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }
  vsync_enabled_ = enabled;
}

void* GLFWWindow::GetNativeWindow() { return static_cast<void*>(window_); }

void GLFWWindow::Destroy()
{
  glfwDestroyWindow(window_);
  glfwTerminate();
}

EventManager* GLFWWindow::GetEventManager() { return event_manager_; }


}// namespace evie