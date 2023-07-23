Evie_add_library(TARGET Window SRCS window.cpp layer_stack.cpp)

target_link_libraries(
  Window
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Logging
  Evie::EventSystem
)

target_link_system_libraries(
  Window
  PRIVATE
  glfw
)