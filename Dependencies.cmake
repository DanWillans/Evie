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
      "b0c9ad189a9b0700cd0cd5d686c1d89dcafcc48f" # This has a fix for installing FMT.
      GITHUB_REPOSITORY
      "DanWillans/fmt"
      OPTIONS
      "CMAKE_POSITION_INDEPENDENT_CODE ON"
      SYSTEM)
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.14.1
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
      "GLFW_BUILD_TESTS OFF"
      "GLFW_BUILD_DOCS OFF"
      "GLFW_BUILD_EXAMPLES OFF"
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

    # This include is needed for global glm access
    include_directories(SYSTEM ${glm_SOURCE_DIR})
  endif()

  if(NOT TARGET ankerl::unordered_dense)
    cpmaddpackage(
      NAME
      ankerl
      GIT_TAG
      v4.4.0
      GITHUB_REPOSITORY
      "martinus/unordered_dense"
      SYSTEM)
    include_directories(SYSTEM ${ankerl_SOURCE_DIR}/include)
  endif()

  if(NOT TARGET assimp::assimp)
  cpmaddpackage(
    NAME
    assimp
    GIT_TAG
    v5.4.2
    GITHUB_REPOSITORY
    "assimp/assimp"
    OPTIONS
    "ASSIMP_WARNINGS_AS_ERRORS OFF"
    SYSTEM)
  include_directories(SYSTEM ${Assimp_SOURCE_DIR}/include)
  endif()
endfunction()
