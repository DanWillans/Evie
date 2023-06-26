evie_add_library(TARGET Logging SRCS logging.cpp)

target_link_libraries(
  Logging
  PRIVATE Evie::Evie_options
  Evie::Evie_warnings
)

target_link_system_libraries(
  Logging
  PRIVATE spdlog::spdlog)