#include <memory>
#include <stdio.h>
#include <string>
#include <vector>


#include <Windows.h>
// Need this space between glfw and windows otherwise there is a macro redefinition error

#include "GLFW/glfw3.h"

#include "event_system//key_events.h"
#include "event_system/event_manager.h"
#include "evie/error.h"
#include "evie/logging.h"
#include "window/window.h"


namespace evie {
namespace {
  static void KeyCallback([[maybe_unused]] GLFWwindow* window,
    int key,
    [[maybe_unused]] int scancode,
    int action,
    [[maybe_unused]] int mods)// NOLINT
  {
    auto* user_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS) {
      std::unique_ptr<KeyPressedEvent> key_pressed_event = std::make_unique<KeyPressedEvent>(key, 0);
      if (user_window != nullptr) {
        auto* event_listener = user_window->GetEventListener();
        if (event_listener != nullptr) {
          event_listener->OnEvent(std::move(key_pressed_event));
        } else {
          EV_WARN("No valid event_listener, dropping event");
        }
      } else {
        EV_WARN("Invalid user window pointer");
      }
    };
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
  // Setup key callbacks
  glfwSetKeyCallback(window_, KeyCallback);
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