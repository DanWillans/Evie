Evie_add_library(TARGET Application SRCS application.cpp)

target_link_system_libraries(
  Application
  PRIVATE
  Evie::Logging
)