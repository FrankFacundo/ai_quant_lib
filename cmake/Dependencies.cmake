include(FetchContent)

# Default to OFF in constrained environments; enable to download automatically.
set(QAI_USE_FETCH_EIGEN OFF CACHE BOOL "Fetch Eigen if not found")
set(QAI_USE_FETCH_GTEST OFF CACHE BOOL "Fetch GTest if not found")
set(QAI_USE_FETCH_PYBIND OFF CACHE BOOL "Fetch pybind11 if not found")

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
if(NOT Eigen3_FOUND)
  message(FATAL_ERROR "Eigen3 not found. Install system Eigen3 or enable QAI_USE_FETCH_EIGEN=ON.")
endif()

if(BUILD_TESTING)
  if(QAI_USE_FETCH_GTEST)
    message(STATUS "Fetching GTest")
    FetchContent_Declare(
      gtest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(gtest)
  else()
    find_package(GTest QUIET)
  endif()
  if(NOT TARGET GTest::gtest_main)
    if(TARGET gtest_main)
      add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
  endif()
  if(NOT TARGET GTest::gtest)
    if(TARGET gtest)
      add_library(GTest::gtest ALIAS gtest)
    endif()
  endif()
  if(NOT TARGET GTest::gtest_main)
    message(FATAL_ERROR "GTest not found. Install libgtest-dev or enable QAI_USE_FETCH_GTEST=ON.")
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
  if(NOT pybind11_FOUND AND NOT TARGET pybind11::module)
    message(FATAL_ERROR "pybind11 not found. Install pybind11-dev or enable QAI_USE_FETCH_PYBIND=ON.")
  endif()
endif()
