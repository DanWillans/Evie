evie_add_library(TARGET Logging SRCS logging.cpp)

target_link_system_libraries(
  Logging
  PRIVATE
  spdlog::spdlog)