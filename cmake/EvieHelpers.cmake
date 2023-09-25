function(Evie_add_library)
  set(options STATIC)
  set(oneValueArgs TARGET)
  set(multiValueArgs SRCS)
  cmake_parse_arguments(
    PARSED_ARG
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  if(NOT PARSED_ARG_TARGET)
    message(FATAL_ERROR "You must provide a target name. Provide TARGET in your function call.")
  endif()
  STRING(TOLOWER ${PARSED_ARG_TARGET} TARGET_LOWER)

  if(NOT PARSED_ARG_SRCS)
    message(FATAL_ERROR "You must provide at least one source file. Provide SRCS in your function call.")
  endif()

  if(NOT PARSED_ARG_STATIC)
    message(STATUS "Defaulting to shared lib for ${PARSED_ARG_TARGET}")
    add_library(${PARSED_ARG_TARGET} SHARED ${PARSED_ARG_SRCS})
  else()
    add_library(${PARSED_ARG_TARGET} STATIC ${PARSED_ARG_SRCS})
  endif()

  set(ALIAS_TARGET Evie::${PARSED_ARG_TARGET})
  add_library(${ALIAS_TARGET} ALIAS ${PARSED_ARG_TARGET})

  target_include_directories(${PARSED_ARG_TARGET} ${WARNING_GUARD} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}> # Sometimes headers are in the source dir
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

  target_compile_features(${PARSED_ARG_TARGET} PUBLIC cxx_std_20)

  Evie_target_export_definitions(${PARSED_ARG_TARGET} PUBLIC)

  if(WIN32)
    set(OUTPUT_NAME libevie_${TARGET_LOWER})
    if(NOT PARSED_ARG_STATIC)
      set(SUFFIX ".dll")
    else()
      set(SUFFIX ".lib")
    endif()
  else()
    set(OUTPUT_NAME evie_${TARGET_LOWER})
    if(NOT PARSED_ARG_STATIC)
      set(SUFFIX ".so")
    else()
      set(SUFFIX ".a")
    endif()
  endif()

  set_target_properties(
    ${PARSED_ARG_TARGET}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    OUTPUT_NAME ${OUTPUT_NAME}
    SUFFIX ${SUFFIX})
endfunction()

function(Evie_target_export_definitions target)
  set(multiValueArgs INTERFACE PUBLIC PRIVATE)
  cmake_parse_arguments(
    ARG
    ""
    ""
    "${multiValueArgs}"
    ${ARGN})

  foreach(scope IN ITEMS INTERFACE PUBLIC PRIVATE)
    # Platform specific definitions
    if(WIN32)
      target_compile_definitions(${target} ${scope} EVIE_PLATFORM_WINDOWS)
      target_compile_definitions(${target} ${scope} EVIE_BUILD_SHARED_LIBRARY)
    elseif(APPLE)
      target_compile_definitions(${target} ${scope} EVIE_PLATFORM_APPLE)
    elseif(UNIX)
      target_compile_definitions(${target} ${scope} EVIE_PLATFORM_UNIX)
    else()
    endif()
  endforeach()
endfunction()
