Evie_add_library(TARGET application SRCS application.cpp)

target_link_system_libraries(
  application
  PRIVATE
    spdlog::spdlog
    evie::logging)