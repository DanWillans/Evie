find_package(OpenGL REQUIRED)
# Maybe don't do this. It's not an Evie library just issue an ImGui instance for users to use alongside Evie.
Evie_add_library(
  TARGET
    ImGui
  SRCS
    imgui_draw.cpp
    imgui_impl_opengl3.cpp
    imgui_impl_glfw.cpp
    imgui_stdlib.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui_demo.cpp
  STATIC
)

set_target_properties(ImGui PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_system_libraries(ImGui PUBLIC glfw OpenGL::GL)