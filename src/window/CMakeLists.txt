find_package(OpenGL REQUIRED)

Evie_add_library(
  TARGET
    Window
  SRCS
    layer_queue.cpp
    event_manager.cpp
    debug_layer.cpp
    glfw_window.cpp
    input_manager_impl.cpp
)

target_link_libraries(
  Window
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Logging
)

target_link_system_libraries(
  Window
  PUBLIC
  glfw
  Evie::ImGui
  OpenGL::GL
  Evie::Glad
)