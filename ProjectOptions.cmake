include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(Evie_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(Evie_setup_options)
  option(Evie_ENABLE_HARDENING "Enable hardening" ON)
  option(Evie_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    Evie_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    Evie_ENABLE_HARDENING
    OFF)

  Evie_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR Evie_PACKAGING_MAINTAINER_MODE)
    option(Evie_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(Evie_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(Evie_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Evie_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Evie_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Evie_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(Evie_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(Evie_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Evie_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(Evie_ENABLE_IPO "Enable IPO/LTO" ON)
    option(Evie_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(Evie_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Evie_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(Evie_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(Evie_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Evie_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Evie_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Evie_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(Evie_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(Evie_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Evie_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      Evie_ENABLE_IPO
      Evie_WARNINGS_AS_ERRORS
      Evie_ENABLE_USER_LINKER
      Evie_ENABLE_SANITIZER_ADDRESS
      Evie_ENABLE_SANITIZER_LEAK
      Evie_ENABLE_SANITIZER_UNDEFINED
      Evie_ENABLE_SANITIZER_THREAD
      Evie_ENABLE_SANITIZER_MEMORY
      Evie_ENABLE_UNITY_BUILD
      Evie_ENABLE_CLANG_TIDY
      Evie_ENABLE_CPPCHECK
      Evie_ENABLE_COVERAGE
      Evie_ENABLE_PCH
      Evie_ENABLE_CACHE)
  endif()

  Evie_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (Evie_ENABLE_SANITIZER_ADDRESS OR Evie_ENABLE_SANITIZER_THREAD OR Evie_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(Evie_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(Evie_global_options)
  if(Evie_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    Evie_enable_ipo()
  endif()

  Evie_supports_sanitizers()

  if(Evie_ENABLE_HARDENING AND Evie_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR Evie_ENABLE_SANITIZER_UNDEFINED
       OR Evie_ENABLE_SANITIZER_ADDRESS
       OR Evie_ENABLE_SANITIZER_THREAD
       OR Evie_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${Evie_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${Evie_ENABLE_SANITIZER_UNDEFINED}")
    Evie_enable_hardening(Evie_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(Evie_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(Evie_warnings INTERFACE)
  add_library(Evie_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  Evie_set_project_warnings(
    Evie_warnings
    ${Evie_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(Evie_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(Evie_options)
  endif()

  include(cmake/Sanitizers.cmake)
  Evie_enable_sanitizers(
    Evie_options
    ${Evie_ENABLE_SANITIZER_ADDRESS}
    ${Evie_ENABLE_SANITIZER_LEAK}
    ${Evie_ENABLE_SANITIZER_UNDEFINED}
    ${Evie_ENABLE_SANITIZER_THREAD}
    ${Evie_ENABLE_SANITIZER_MEMORY})

  set_target_properties(Evie_options PROPERTIES UNITY_BUILD ${Evie_ENABLE_UNITY_BUILD})

  if(Evie_ENABLE_PCH)
    target_precompile_headers(
      Evie_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(Evie_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    Evie_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(Evie_ENABLE_CLANG_TIDY)
    Evie_enable_clang_tidy(Evie_options ${Evie_WARNINGS_AS_ERRORS})
  endif()

  if(Evie_ENABLE_CPPCHECK)
    Evie_enable_cppcheck(${Evie_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(Evie_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    Evie_enable_coverage(Evie_options)
  endif()

  if(Evie_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(Evie_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(Evie_ENABLE_HARDENING AND NOT Evie_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR Evie_ENABLE_SANITIZER_UNDEFINED
       OR Evie_ENABLE_SANITIZER_ADDRESS
       OR Evie_ENABLE_SANITIZER_THREAD
       OR Evie_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    Evie_enable_hardening(Evie_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
