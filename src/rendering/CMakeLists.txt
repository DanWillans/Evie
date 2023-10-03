find_package(OpenGL REQUIRED)

Evie_add_library(
  TARGET
  Rendering
  SRCS
  shader_program.cpp
)

target_link_libraries(
  Rendering
  PUBLIC
  Evie::Evie_options
  Evie::Evie_warnings
)

target_link_system_libraries(
  Rendering
  PRIVATE
  Evie::Glad
  Evie::Logging
  OpenGL::GL
)