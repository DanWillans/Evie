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
      "CMAKE_POSITION_INDEPENDENT_CODE ON"
      SYSTEM)
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
      "CMAKE_POSITION_INDEPENDENT_CODE ON"
      SYSTEM)

    # This include is needed for global logging access
    include_directories(SYSTEM ${spdlog_SOURCE_DIR}/include)
  endif()

  if(NOT TARGET doctest::doctest)
    cpmaddpackage("gh:doctest/doctest@2.4.11")
  endif()

  if(NOT TARGET glfw)
    cpmaddpackage(
      NAME
      glfw
      GIT_TAG
      3.3.8
      GITHUB_REPOSITORY
      "glfw/glfw"
      OPTIONS
      "BUILD_SHARED_LIBS ON"
      SYSTEM)
  endif()

  if(NOT TARGET glm::glm)
    cpmaddpackage(
      NAME
      glm
      GIT_TAG
      0.9.9.8
      GITHUB_REPOSITORY
      "g-truc/glm"
      SYSTEM)
  endif()

endfunction()
