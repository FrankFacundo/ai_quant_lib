if(NOT BUILD_PYTHON)
  return()
endif()

# pybind11 may already be provided via FetchContent in Dependencies.cmake
if(NOT TARGET pybind11::module)
  find_package(pybind11 CONFIG REQUIRED)
endif()

pybind11_add_module(pyquant bindings/pyquant.cpp)

target_link_libraries(pyquant PRIVATE quantlib Eigen3::Eigen)

target_compile_features(pyquant PRIVATE cxx_std_20)

set_target_properties(pyquant PROPERTIES
  OUTPUT_NAME pyquant
  LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/python
)
