evie_add_library(TARGET logging SRCS loggers.cpp)

target_link_system_libraries(
  logging
  PRIVATE
    spdlog::spdlog)