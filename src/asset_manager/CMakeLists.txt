Evie_add_library(TARGET AssetManager SRCS asset_manager.cpp)

target_link_libraries(
  AssetManager
  PRIVATE
  Evie::Evie_options
  Evie::Evie_warnings
  Evie::WhereAmI
  Evie::Rendering
  PUBLIC
  Evie::Logging
)

target_link_system_libraries(
  AssetManager
  PUBLIC
  Evie::Glad
)
