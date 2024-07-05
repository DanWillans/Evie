cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----
if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(Evie CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET Evie_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----
find_package(doctest REQUIRED)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
# TODO: Bring this back
# add_test(NAME cli.has_help COMMAND intro --help)

# Provide a test to verify that the version being reported from the application
# matches the version given to CMake. This will be important once you package
# your program. Real world shows that this is the kind of simple mistake that is easy
# to make, but also easy to test for.
# TODO: Bring this back
# add_test(NAME cli.version_matches COMMAND intro --version)
# set_tests_properties(cli.version_matches PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")

# ##### EVENT SYSTEM TESTS ######
add_executable(event_system_tests main.cpp event_system_tests.cpp)
target_link_libraries(
  event_system_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::Window
  doctest::doctest)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  event_system_tests
  TEST_PREFIX
  "EventSystemUnittests."
)

if(WIN32)
  add_custom_command(
    TARGET event_system_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:event_system_tests> $<TARGET_FILE_DIR:event_system_tests>
    COMMAND_EXPAND_LISTS)
endif()

# ###### Error Tests ########
add_executable(error_tests main.cpp error_tests.cpp)
target_link_libraries(
  error_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  doctest::doctest)
cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH EVIE_PROJECT_ROOT)
target_include_directories(error_tests PUBLIC $<BUILD_INTERFACE:${EVIE_PROJECT_ROOT}/include>)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  error_tests
  TEST_PREFIX
  "ErrorUnittests."
)

# ###### Layer Queue Tests ########
add_executable(layer_queue_tests main.cpp layer_queue_tests.cpp)
target_link_libraries(
  layer_queue_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::Window
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET layer_queue_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:layer_queue_tests> $<TARGET_FILE_DIR:layer_queue_tests>
    COMMAND_EXPAND_LISTS)
endif()


# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  layer_queue_tests
  TEST_PREFIX
  "LayerQueueUnittests."
)

# ###### Input Manager Tests ########
add_executable(input_manager_tests main.cpp input_manager_tests.cpp)
target_link_libraries(
  input_manager_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::Window
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET input_manager_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:input_manager_tests> $<TARGET_FILE_DIR:input_manager_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  input_manager_tests
  TEST_PREFIX
  "InputManagerUnittests."
)

# ###### Result Tests ########
add_executable(result_tests main.cpp result_tests.cpp)
target_link_libraries(
  result_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  doctest::doctest)
cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH EVIE_PROJECT_ROOT)
target_include_directories(result_tests PUBLIC $<BUILD_INTERFACE:${EVIE_PROJECT_ROOT}/include>)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  result_tests
  TEST_PREFIX
  "ResultUnittests."
)

###### Component Manager Tests ########
add_executable(component_manager_tests main.cpp component_manager_tests.cpp)
target_link_libraries(
  component_manager_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::EntityComponentSystem
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET component_manager_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:component_manager_tests> $<TARGET_FILE_DIR:component_manager_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  component_manager_tests
  TEST_PREFIX
  "ComponentManagerUnittests."
)

###### System Manager Tests ########
add_executable(system_manager_tests main.cpp system_manager_tests.cpp)
target_link_libraries(
  system_manager_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::EntityComponentSystem
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET system_manager_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:system_manager_tests> $<TARGET_FILE_DIR:system_manager_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  system_manager_tests
  TEST_PREFIX
  "SystemManagerUnittests."
)

###### Entity Manager Tests ########
add_executable(entity_manager_tests main.cpp entity_manager_tests.cpp)
target_link_libraries(
  entity_manager_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::EntityComponentSystem
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET entity_manager_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:entity_manager_tests> $<TARGET_FILE_DIR:entity_manager_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  entity_manager_tests
  TEST_PREFIX
  "EntityManagerUnittests."
)

###### ECS Controller Tests ########
add_executable(ecs_controller_tests main.cpp ecs_controller_tests.cpp)
target_link_libraries(
  ecs_controller_tests
  PRIVATE
  Evie::Evie_warnings
  Evie::Evie_options
  Evie::EntityComponentSystem
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET ecs_controller_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ecs_controller_tests> $<TARGET_FILE_DIR:ecs_controller_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  ecs_controller_tests
  TEST_PREFIX
  "ECSControllerUnittests."
)