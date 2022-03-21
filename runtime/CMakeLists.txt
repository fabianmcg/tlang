cmake_minimum_required(VERSION 3.10)

project(tlang-runtime VERSION 0.42 LANGUAGES CXX)

set(CMAKE_C_COMPILER "clang-13")
set(CMAKE_CXX_COMPILER "clang++-13")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR})

aux_source_directory(lib/ LIB_SOURCES)

add_library(tlang-runtime STATIC ${LIB_SOURCES})
link_directories(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(tlang-runtime pthread)