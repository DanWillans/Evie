Evie_add_library(
  TARGET
    EntityComponentSystem
  SRCS
    entity_manager.cpp
    component_manager.cpp
    system_manager.cpp
    ecs_controller.cpp
)

target_link_libraries(
  EntityComponentSystem
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
)

target_link_libraries(
  EntityComponentSystem
  PUBLIC
  Evie::Logging
)