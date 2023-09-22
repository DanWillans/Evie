find_package(OpenGL REQUIRED)

Evie_add_library(
  TARGET Window 
  SRCS window.cpp layer_queue.cpp event_manager.cpp debug_layer.cpp)

target_link_libraries(
  Window
  PUBLIC
  Evie::Utilities
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
)