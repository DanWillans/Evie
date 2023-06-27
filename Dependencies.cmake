include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(Evie_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage(
      NAME
      fmt
      GIT_TAG
      9.1.0
      GITHUB_REPOSITORY
      "fmtlib/fmt"
      OPTIONS
      "CMAKE_POSITION_INDEPENDENT_CODE ON")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.11.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON"
      "CMAKE_POSITION_INDEPENDENT_CODE ON")
    # This include is needed for global logging access
    include_directories(SYSTEM ${spdlog_SOURCE_DIR}/include)
  endif()

  if(NOT TARGET doctest::doctest)
    cpmaddpackage("gh:doctest/doctest@2.4.11")
  endif()

endfunction()
