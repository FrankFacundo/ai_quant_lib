cmake_minimum_required(VERSION 3.16)
project(ai_quant_lib VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_PYTHON "Build Python bindings" ON)
option(BUILD_TESTING "Build tests" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Dependencies)

add_subdirectory(src)

if(BUILD_PYTHON)
  add_subdirectory(python)
endif()

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

