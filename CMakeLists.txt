cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME seeppascal)
project(${PROJECT_NAME})

set(PROJ_LIBRARY "${PROJECT_NAME}")
set(PROJ_TESTS   "test_${PROJECT_NAME}")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_subdirectory(interpreter)
add_subdirectory(tools)
add_subdirectory(samples)
add_subdirectory(test)