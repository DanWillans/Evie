#ifdef EVIE_PLATFORM_APPLE
// This is required because Apple has deprecated OpenGL
#define GL_SILENCE_DEPRECATION 1
#endif

#include "window/debug_layer.h"
#include "GLFW/glfw3.h"
#include "evie/events.h"
#include "evie/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


namespace evie {

DebugLayer::DebugLayer(void* window)
{
  GLFWwindow* glfw_window = static_cast<GLFWwindow*>(window);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& imgui_io = ImGui::GetIO();
  imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
    glfw_window, true);// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  EV_INFO("Initialised debug layer");
}

void DebugLayer::OnUpdate()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow();// Show demo window! :)
  ImGui::Render();
  // I don't think this should be here. I imagine the render should handle clearing the buffer.
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugLayer::OnEvent([[maybe_unused]] Event& event)
{
  const ImGuiIO& imgui_io = ImGui::GetIO();
  // Make sure the event is handled if imgui wanted it
  if (imgui_io.WantCaptureMouse) {
    event.handled = true;
  }
}

void DebugLayer::Shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

}// namespace evie