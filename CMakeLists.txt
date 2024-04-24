cmake_minimum_required(VERSION 3.20)

include(GoogleTest)

project(dns_cache LANGUAGES CXX )
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
include(CMakePrintHelpers)
cmake_print_variables(TEST_SOURCES)

add_executable(tests ${TEST_SOURCES})
enable_testing()
gtest_discover_tests(tests)
target_link_libraries(tests gtest)
