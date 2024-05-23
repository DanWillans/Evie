cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(DanDan
  VERSION 1.0
  DESCRIPTION "A game where DanDans come for you"
  HOMEPAGE_URL "https://github.com/DanWillans/Evie"
  LANGUAGES CXX C)

include_directories(include)

add_executable(dan_dan_the_game src/main.cpp)

target_link_libraries(
  dan_dan_the_game
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