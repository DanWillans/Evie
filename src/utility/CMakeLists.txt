add_library(Utilities INTERFACE)
add_library(Evie::Utilities ALIAS Utilities)
target_include_directories(Utilities INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
                                            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)