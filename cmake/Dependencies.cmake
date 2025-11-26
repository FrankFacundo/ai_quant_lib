include(FetchContent)

set(QAI_USE_FETCH_EIGEN ON CACHE BOOL "Fetch Eigen if not found")
set(QAI_USE_FETCH_GTEST ON CACHE BOOL "Fetch GTest if not found")
set(QAI_USE_FETCH_PYBIND ON CACHE BOOL "Fetch pybind11 if not found")

find_package(Eigen3 QUIET)
if(NOT Eigen3_FOUND AND QAI_USE_FETCH_EIGEN)
  message(STATUS "Fetching Eigen3")
  FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
  )
  FetchContent_MakeAvailable(eigen)
endif()
if(NOT Eigen3_FOUND AND TARGET eigen)
  set(Eigen3_FOUND ON)
  add_library(Eigen3::Eigen ALIAS eigen)
endif()

if(BUILD_TESTING)
  find_package(GTest QUIET)
  if(NOT GTest_FOUND AND QAI_USE_FETCH_GTEST)
    message(STATUS "Fetching GTest")
    FetchContent_Declare(
      gtest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(gtest)
  endif()
endif()

if(BUILD_PYTHON)
  find_package(pybind11 QUIET)
  if(NOT pybind11_FOUND AND QAI_USE_FETCH_PYBIND)
    message(STATUS "Fetching pybind11")
    FetchContent_Declare(
      pybind11
      GIT_REPOSITORY https://github.com/pybind/pybind11.git
      GIT_TAG v2.10.4
    )
    FetchContent_MakeAvailable(pybind11)
  endif()
endif()

