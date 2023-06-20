cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(Sandbox
        VERSION 1.0
        DESCRIPTION "A sandbox playground for the Evie game engine"
        HOMEPAGE_URL "https://github.com/DanWillans/Evie"
        LANGUAGES CXX C)

add_executable(sandbox src/main.cpp)

target_link_libraries(sandbox PUBLIC evie::application evie::logging)