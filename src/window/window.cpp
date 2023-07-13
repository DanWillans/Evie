#include <memory>
#include <stdio.h>
#include <string>
#include <vector>


#include <Windows.h>
// Need this space between glfw and windows otherwise there is a macro redefinition error

#include "GLFW/glfw3.h"

#include "event_system//key_events.h"
#include "event_system/event_manager.h"
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
  void KeyCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      // I'm not sure I like how we handle repeat keys in this function but if it becomes a problem then we'll handle
      // it in the future. Also if this ever becomes multi threaded then it needs fixing.
      static int repeat_count{ 0 };
      std::unique_ptr<KeyEvent> key_event;
      switch (action) {
      case GLFW_PRESS:
        repeat_count = 0;
        key_event = std::make_unique<KeyPressedEvent>(key, 0);
        break;
      case GLFW_RELEASE:
        repeat_count = 0;
        key_event = std::make_unique<KeyReleasedEvent>(key);
        break;
      case GLFW_REPEAT:
        repeat_count++;
        key_event = std::make_unique<KeyPressedEvent>(key, repeat_count);
        break;
      default:
        EV_WARN("Unsupported key action {}", action);
      }
      if (key_event) {
        event_listener->OnEvent(std::move(key_event));
      }
    }
  }

  void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<MouseMovementEvent> mouse_movement_event =
        std::make_unique<MouseMovementEvent>(MousePosition{ xpos, ypos });
      event_listener->OnEvent(std::move(mouse_movement_event));
    }
  }

  void MouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<Event> mouse_event;
      switch (action) {
      case GLFW_PRESS:
        mouse_event = std::make_unique<MousePressedEvent>(static_cast<MouseButton>(button));
        break;
      case GLFW_RELEASE:
        mouse_event = std::make_unique<MouseReleasedEvent>(static_cast<MouseButton>(button));
        break;
      default:
        EV_WARN("Unsupported mouse action {}", action);
      }
      if (mouse_event) {
        event_listener->OnEvent(std::move(mouse_event));
      }
    }
  }

  void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<MouseScrolledEvent> mouse_scroll_event =
        std::make_unique<MouseScrolledEvent>(MouseScrollOffset{ xoffset, yoffset });
      event_listener->OnEvent(std::move(mouse_scroll_event));
    }
  }

  void WindowFocusCallback(GLFWwindow* window, int focused)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<WindowEvent> window_event;
      if (focused == 1) {
        window_event = std::make_unique<WindowFocusEvent>();
      } else {
        window_event = std::make_unique<WindowLostFocusEvent>();
      }
      event_listener->OnEvent(std::move(window_event));
    }
  }

  void WindowSizeCallback(GLFWwindow* window, int width, int height)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<WindowResizeEvent> window_event =
        std::make_unique<WindowResizeEvent>(WindowDimensions{ width, height });
      event_listener->OnEvent(std::move(window_event));
    }
  }

  void WindowCloseCallback(GLFWwindow* window)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<WindowCloseEvent> window_event = std::make_unique<WindowCloseEvent>();
      event_listener->OnEvent(std::move(window_event));
    }
  }

  void WindowPositionCallback(GLFWwindow* window, int xpos, int ypos)
  {
    auto* event_listener = GetEventListener(window);
    if (event_listener != nullptr) {
      std::unique_ptr<WindowMovedEvent> window_moved_event =
        std::make_unique<WindowMovedEvent>(WindowPosition{ xpos, ypos });
      event_listener->OnEvent(std::move(window_moved_event));
    }
  }
}// namespace

class Window::Impl
{
public:
  explicit Impl(const char* name, Window* parent_window) : window_name_(name), parent_window_ptr_(parent_window) {}
  Error Initialise();
  void Update();
  Error RegisterEventListener(IEventListener* event_listener);
  void SetupInputCallbacks();
  IEventListener* GetEventListener() { return event_listener_; };

private:
  Error DispatchEvent(std::unique_ptr<Event> event);
  GLFWwindow* window_{ nullptr };
  const char* window_name_;
  // This could be public if we allow people to overwrite the event listener
  IEventListener* event_listener_{ nullptr };
  // Required to set user window pointer
  Window* parent_window_ptr_{ nullptr };
};

Window::Window(const char* window_name) : impl_(new Impl(window_name, this)) {}

Error Window::Initialise() { return impl_->Initialise(); }

void Window::Update() { return impl_->Update(); }

Error Window::RegisterEventListener(IEventListener* event_listener)
{
  return impl_->RegisterEventListener(event_listener);
}

IEventListener* Window::GetEventListener() { return impl_->GetEventListener(); }

Window::~Window()
{
  glfwTerminate();
  delete impl_;
}

Error Window::Impl::Initialise()
{
  if (glfwInit() == 0) {
    return Error("glfw failed to initialise");
  }
  window_ = glfwCreateWindow(640, 480, window_name_, nullptr, nullptr);
  if (window_ == nullptr) {
    return Error("glfw failed to create window");
  }
  glfwMakeContextCurrent(window_);
  // If we use glad we should init it here.
  glfwSwapInterval(1);
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


}// namespace evie