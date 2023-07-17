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
  PRIVATE Evie::Evie_warnings
  Evie::Evie_options
  Evie::EventSystem
  doctest::doctest)

if(WIN32)
  add_custom_command(
    TARGET event_system_tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:event_system_tests> $<TARGET_FILE_DIR:event_system_tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
doctest_discover_tests(
  event_system_tests
  TEST_PREFIX
  "unittests."
)

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
  "unittests."
)