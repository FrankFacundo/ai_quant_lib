file(GLOB TEST_SOURCES unit/*.cpp)

add_executable(quant_tests ${TEST_SOURCES})

find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED)

target_link_libraries(quant_tests PRIVATE quantlib GTest::gtest_main Eigen3::Eigen)

target_include_directories(quant_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_features(quant_tests PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(quant_tests)

