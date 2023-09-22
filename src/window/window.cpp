#include <memory>
#include <stdio.h>
#include <string>
#include <vector>


#ifdef EVIE_PLATFORM_WINDOWS
#include <Windows.h>
#endif
// Need this space between glfw and windows otherwise there is a macro redefinition error

#include "GLFW/glfw3.h"

#include "evie/error.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "window/event_manager.h"
#include "window/key_events.h"
#include "window/mouse_events.h"
#include "window/window.h"
#include "window/window_events.h"


namespace evie {
namespace {

  static void glfw_error_callback(int error, const char* description)
  {
    EV_ERROR("GLFW Error {}: {}", error, description);
  }

  EventManager* GetEventManager(GLFWwindow* window)
  {
    auto* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
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
        DispatchEvent<MousePressedEvent>(event_manager, static_cast<MouseButton>(button));
        break;
      case GLFW_RELEASE:
        DispatchEvent<MouseReleasedEvent>(event_manager, static_cast<MouseButton>(button));
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

class Window::Impl
{
public:
  Impl(const WindowProperties& props, Window* parent_window) : properties_(props), parent_window_ptr_(parent_window) {}
  Error Initialise();
  static void PollEvents();
  void SwapBuffers();
  Error RegisterEventManager(EventManager& event_manager);
  void SetupInputCallbacks();
  EventManager* GetEventManager() { return event_manager_; };
  void SetVSync(bool enabled);
  GLFWwindow* GetGLFWWindow() { return window_; };
  ~Impl();

private:
  Error DispatchEvent(std::unique_ptr<Event> event);
  GLFWwindow* window_{ nullptr };
  WindowProperties properties_;
  // This could be public if we allow people to overwrite the event manager
  EventManager* event_manager_{ nullptr };
  // Required to set user window pointer
  Window* parent_window_ptr_{ nullptr };
  bool vsync_enabled_{ false };
};

Window::Window(const WindowProperties& window_props) : impl_(new Impl(window_props, this)) {}

Error Window::Initialise() { return impl_->Initialise(); }

void Window::PollEvents() { return impl_->PollEvents(); }
void Window::SwapBuffers() { return impl_->SwapBuffers(); }

Error Window::RegisterEventManager(EventManager& event_manager) { return impl_->RegisterEventManager(event_manager); }

EventManager* Window::GetEventManager() { return impl_->GetEventManager(); }

void Window::SetVSyncFlag(bool enabled) { impl_->SetVSync(enabled); }

GLFWwindow* Window::GetGLFWWindow() { return impl_->GetGLFWWindow(); }

Window::~Window() { delete impl_; }

Window::Impl::~Impl() { glfwTerminate(); }

Error Window::Impl::Initialise()
{
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
  glfwSetWindowUserPointer(window_, static_cast<void*>(parent_window_ptr_));
  SetupInputCallbacks();
  return Error::OK();
}

void Window::Impl::SetupInputCallbacks()
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

void Window::Impl::PollEvents() { glfwPollEvents(); }

void Window::Impl::SwapBuffers() { glfwSwapBuffers(window_); }

Error Window::Impl::RegisterEventManager(EventManager& event_manager)
{
  if (event_manager_ == nullptr) {
    event_manager_ = &event_manager;
  } else {
    return Error("EventManager already exists. Only one event manager can be registered for now");
  }
  return Error::OK();
};

void Window::Impl::SetVSync(bool enabled)
{
  if (enabled) {
    glfwSwapInterval(1);
  } else {
    glfwSwapInterval(0);
  }
  vsync_enabled_ = enabled;
}

}// namespace evie