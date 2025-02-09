cmake_minimum_required(VERSION 3.22)

project(QSDB)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3")

set(GTEST_ROOT "/home/zhangsan/code/third_party/gtest")
set(GTEST_INCLUDE_DIRS "${GTEST_ROOT}/include")
set(GTEST_LIBRARIES "${GTEST_ROOT}/lib")

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(example)
