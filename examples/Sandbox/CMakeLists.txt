cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(Sandbox
  VERSION 1.0
  DESCRIPTION "A sandbox playground for the Evie game engine"
  HOMEPAGE_URL "https://github.com/DanWillans/Evie"
  LANGUAGES CXX C)

add_executable(spinning_box_example src/spinning_box_example.cpp)

target_link_libraries(
  spinning_box_example
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Application
  Evie::Logging
  Evie::Rendering
  Evie::EntityComponentSystem
  PUBLIC
  # This is just for Time. Make Evie::Time.
  glfw
)

add_executable(lighting_example src/lighting_example.cpp)

target_link_libraries(
  lighting_example
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Application
  Evie::Logging
  Evie::Rendering
  Evie::EntityComponentSystem
  Evie::ImGui
  PUBLIC
  # This is just for Time. Make Evie::Time.
  glfw
)

add_executable(all_lighting_example src/all_lighting_example.cpp)

target_link_libraries(
  all_lighting_example
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Application
  Evie::Logging
  Evie::Rendering
  Evie::EntityComponentSystem
  Evie::ImGui
  PUBLIC
  # This is just for Time. Make Evie::Time.
  glfw
)

install(DIRECTORY assets DESTINATION ${CMAKE_INSTALL_PREFIX})