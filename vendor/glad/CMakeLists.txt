Evie_add_library(TARGET Glad SRCS src/glad.c)
target_include_directories(Glad INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
target_compile_definitions(Glad PUBLIC GLAD_GLAPI_EXPORT PRIVATE GLAD_GLAPI_EXPORT_BUILD)