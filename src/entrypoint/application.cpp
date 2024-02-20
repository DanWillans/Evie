#include <imgui_internal.h>
#include <memory>
#include <thread>

#include "evie/application.h"
#include "evie/camera.h"
#include "evie/ecs/ecs_controller.h"
#include "evie/error.h"
#include "evie/events.h"
#include "evie/input_manager.h"
#include "evie/logging.h"
#include "evie/window.h"
#include "rendering/debug.h"
#include "window/debug_layer.h"
#include "window/event_manager.h"
#include "window/input_manager_impl.h"
#include "window/layer_queue.h"

#include "glad/glad.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
#include "window/glfw_window.h"
#endif

namespace evie {

// =========================
// Impl
class Application::Impl
{
public:
  void PushLayerFront(Layer& layer);
  void PushLayerBack(Layer& layer);

private:
  friend Application;
  std::unique_ptr<IWindow> window_;
  std::unique_ptr<EventManager> event_manager_;
  std::unique_ptr<Layer> debug_layer_;
  std::unique_ptr<IInputManager> input_manager_;
  std::unique_ptr<ECSController> ecs_controller_;
  LayerQueue layer_queue_;
  Camera camera_;
};

void Application::Impl::PushLayerFront(Layer& layer) { layer_queue_.PushFront(layer); }
void Application::Impl::PushLayerBack(Layer& layer) { layer_queue_.PushBack(layer); }
// =========================

Application::Application() : impl_(new Impl()) {}
Application::~Application() { delete impl_; }

const IInputManager* Application::GetInputManager() const { return impl_->input_manager_.get(); }

IWindow* Application::GetWindow() const { return impl_->window_.get(); }

ECSController* Application::GetECSController() const { return impl_->ecs_controller_.get(); }

ImGuiContext* Application::GetImGuiContext() const { return ImGui::GetCurrentContext(); }

Error Application::Initialise(const WindowProperties& props)
{
  // Logging
  LoggingManager::Init();
  EV_INFO("Logging Initialised");

  // Input
  // At the minute we only build for these platforms but a different window impementation will be needed for android
  // etc.
#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
  impl_->input_manager_ = std::make_unique<InputManager>();
#endif

  // Window
  // At the minute we only build for these platforms but a different window impementation will be needed for android
  // etc.
#if defined EVIE_PLATFORM_WINDOWS || defined EVIE_PLATFORM_APPLE || defined EVIE_PLATFORM_UNIX
  impl_->window_ = std::make_unique<GLFWWindow>();
  evie::Error err = impl_->window_->Initialise(props);
  // impl_->debug_layer_ = std::make_unique<DebugLayer>(impl_->window_->GetNativeWindow());
  // impl_->PushLayerBack(*impl_->debug_layer_);
#endif

  // Event System
  impl_->event_manager_ = std::make_unique<EventManager>(impl_->layer_queue_, impl_->input_manager_.get());
  // Register CloseWindow so that we can shut the game down.
  impl_->event_manager_->SubscribeToEventType(
    EventType::WindowClose, [this]([[maybe_unused]] Event& event) { this->CloseWindow(); });

  // TODO: Get rid of this and just construct window with the Event Manager
  if (err.Good()) {
    err = impl_->window_->RegisterEventManager(*impl_->event_manager_);
  }

  if (err.Good()) {
    impl_->ecs_controller_ = std::make_unique<ECSController>();
  }

  // Evie supports ImGui natively so set up the context here for other layers to use.
  if (err.Good()) {
    ImGui::CreateContext();
    ImGui::GetCurrentContext();
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
    imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imgui_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(impl_->window_->GetNativeWindow()),
      true);// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
  }

  if (err.Good()) {
    initialised_ = true;
  }

  return err;
}

void Application::Run()
{
  Error err = Error::OK();
  if (!initialised_) {
    EV_ERROR("Application isn't initialised. Please run Initialise() before Run(). Exiting.");
    return;
  }

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_DEPTH_TEST);
  while (running_ && err.Good()) {
    impl_->window_->PollEvents();
    // Move this to the renderer in the future
    glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
    // glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Generally this involves updating physics etc based on events.
    for (const auto& layer_wrapper : impl_->layer_queue_) {
      layer_wrapper.layer->OnUpdate();
    }
    // Let all layers render what they want to render based on the previous on update.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    for (const auto& layer_wrapper : impl_->layer_queue_) {
      layer_wrapper.layer->OnRender();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    const ImGuiIO& imgui_io = ImGui::GetIO();
    if (imgui_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
    impl_->window_->SwapBuffers();
  }

  if (err.Bad()) {
    EV_ERROR("{}", err.Message());
    std::this_thread::sleep_for(std::chrono::duration(std::chrono::seconds(5)));
  }
}

void Application::Shutdown()
{
  // Shutdown layers first as they'll be using contexts from the window like glfw, opengl etc.
  impl_->layer_queue_.Shutdown();
  if (impl_->window_) {
    impl_->window_->Destroy();
  }
}

void Application::PushLayerFront(Layer& layer) { impl_->layer_queue_.PushFront(layer); }

void Application::PushLayerBack(Layer& layer) { impl_->layer_queue_.PushBack(layer); }

}// namespace evie
