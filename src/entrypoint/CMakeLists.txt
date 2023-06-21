Evie_add_library(TARGET Application SRCS application.cpp)

target_link_libraries(
  Application
  PRIVATE
  Evie::Logging
)