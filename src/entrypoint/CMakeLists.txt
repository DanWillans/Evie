find_package(OpenGL REQUIRED)
Evie_add_library(TARGET Application SRCS application.cpp)

target_link_libraries(
  Application
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Logging
  Evie::Window
  Evie::Rendering
  Evie::StbImage
)

target_link_system_libraries(
  Application
  PRIVATE
  glm::glm
)

target_link_libraries(
  Application
  PUBLIC
  # Renderig here at the minute whilt we test. Remove in future.
  Evie::Glad
  OpenGL::GL
)
