#include <memory>
#include <stdio.h>
#include <string>
#include <vector>


#include <Windows.h>
// Need this space between glfw and windows otherwise there is a macro redefinition error

#include "GLFW/glfw3.h"

#include "event_system/event_manager.h"
#include "event_system/events.h"
#include "event_system/key_events.h"
#include "event_system/mouse_events.h"
#include "event_system/window_events.h"
#include "evie/error.h"
#include "evie/logging.h"
#include "window/window.h"


namespace evie {
namespace {

  IEventListener* GetEventListener(GLFWwindow* window)
  {
    auto* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (user_window != nullptr) {
      auto* event_listener = user_window->GetEventListener();
      if (event_listener != nullptr) {
        return event_listener;
      } else {
        EV_WARN("No valid event_listener, dropping event");
      }
    } else {
      EV_WARN("Invalid user window pointer");
    }
    return nullptr;
  }

  template<typename T, typename... Args> void DispatchEvent(IEventListener* listener, Args... args)
  {
    T event(args...);
    listener->OnEvent(event);
  }


  void KeyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      // I'm not sure I like how we handle repeat keys in this function but if it becomes a problem then we'll handle
      // it in the future. Also if this ever becomes multi threaded then it needs fixing.
      static int repeat_count{ 0 };
      switch (action) {
      case GLFW_PRESS:
        repeat_count = 0;
        DispatchEvent<KeyPressedEvent>(event_listener, key, 0);
        break;
      case GLFW_RELEASE:
        repeat_count = 0;
        DispatchEvent<KeyReleasedEvent>(event_listener, key);
        break;
      case GLFW_REPEAT: {
        repeat_count++;
        DispatchEvent<KeyPressedEvent>(event_listener, key, repeat_count);
        break;
      }
      default:
        EV_WARN("Unsupported key action {}", action);
      }
    }
  }

  void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      DispatchEvent<MouseMovementEvent>(event_listener, MousePosition{ xpos, ypos });
    }
  }

  void MouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      switch (action) {
      case GLFW_PRESS:
        DispatchEvent<MousePressedEvent>(event_listener, static_cast<MouseButton>(button));
        break;
      case GLFW_RELEASE:
        DispatchEvent<MouseReleasedEvent>(event_listener, static_cast<MouseButton>(button));
        break;
      default:
        EV_WARN("Unsupported mouse action {}", action);
      }
    }
  }

  void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      DispatchEvent<MouseScrolledEvent>(event_listener, MouseScrollOffset{ xoffset, yoffset });
    }
  }

  void WindowFocusCallback(GLFWwindow* window, int focused)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      if (focused == 1) {
        DispatchEvent<WindowFocusEvent>(event_listener);
      } else {
        DispatchEvent<WindowLostFocusEvent>(event_listener);
      }
    }
  }

  void WindowSizeCallback(GLFWwindow* window, int width, int height)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      DispatchEvent<WindowResizeEvent>(event_listener, WindowDimensions{ width, height });
    }
  }

  void WindowCloseCallback(GLFWwindow* window)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      DispatchEvent<WindowCloseEvent>(event_listener);
    }
  }

  void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      DispatchEvent<WindowMovedEvent>(event_listener, WindowPosition{ xpos, ypos });
    }
  }
}// namespace

class Window::Impl
{
public:
  Impl(const WindowProperties& props, Window* parent_window) : properties_(props), parent_window_ptr_(parent_window) {}
  Error Initialise();
  void Update();
  Error RegisterEventListener(IEventListener* event_listener);
  void SetupInputCallbacks();
  IEventListener* GetEventListener() { return event_listener_; };
  void SetVSync(bool enabled);

private:
  Error DispatchEvent(std::unique_ptr<Event> event);
  GLFWwindow* window_{ nullptr };
  WindowProperties properties_;
  // This could be public if we allow people to overwrite the event listener
  IEventListener* event_listener_{ nullptr };
  // Required to set user window pointer
  Window* parent_window_ptr_{ nullptr };
  bool vsync_enabled_{ false };
};

Window::Window(const WindowProperties& window_props) : impl_(new Impl(window_props, this)) {}

Error Window::Initialise() { return impl_->Initialise(); }

void Window::Update() { return impl_->Update(); }

Error Window::RegisterEventListener(IEventListener* event_listener)
{
  return impl_->RegisterEventListener(event_listener);
}

IEventListener* Window::GetEventListener() { return impl_->GetEventListener(); }

void Window::SetVSyncFlag(bool enabled) { impl_->SetVSync(enabled); }

Window::~Window()
{
  glfwTerminate();
  delete impl_;
}

Error Window::Impl::Initialise()
{
  // We may want more than one window so we should fix this if we do. Maybe use a static?
  if (glfwInit() == 0) {
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

void Window::Impl::Update()
{
  glfwSwapBuffers(window_);
  glfwPollEvents();
}

Error Window::Impl::RegisterEventListener(IEventListener* event_listener)
{
  if (event_listener_ == nullptr) {
    event_listener_ = event_listener;
  } else {
    return Error("EventListener already exists. Only one event listener can be registered for now");
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