set(target ${PROJ_TESTS})

file(GLOB hdrs "*.h*")
file(GLOB srcs "*.cpp")

add_executable(${target} ${srcs} ${hdrs})

include_directories("${CMAKE_SOURCE_DIR}/gtest")
target_link_libraries(${target} gtest ${PROJ_LIBRARY})