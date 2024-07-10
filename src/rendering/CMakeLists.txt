find_package(OpenGL REQUIRED)

Evie_add_library(
  TARGET
  Rendering
  SRCS
  shader_program.cpp
  texture.cpp
  camera.cpp
  indices_array.cpp
  debug.cpp
  model.cpp
)

target_link_libraries(
  Rendering
  PUBLIC
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Window
)

target_link_system_libraries(
  Rendering
  PRIVATE
  Evie::StbImage
  Evie::Glad
  Evie::Logging
  OpenGL::GL
  glm::glm
  assimp::assimp
)

target_include_directories(
  Rendering
  PRIVATE
  assimp::assimp
)