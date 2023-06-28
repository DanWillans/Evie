Evie_add_library(TARGET EventSystem SRCS src/event_manager.cpp)

target_link_libraries(
  EventSystem
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::Logging
)