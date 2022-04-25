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

link_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(tlang-runtime STATIC lib/parallel.cc)
target_link_libraries(tlang-runtime pthread)

#add_library(tlang-nvptx STATIC lib/nvptx.cc)

find_package(CUDAToolkit)
#target_include_directories(tlang-nvptx PRIVATE )
#target_link_libraries(tlang-nvptx pthread CUDA::cuda_driver)

add_library(tlang-device-nvptx STATIC lib/tlang-device-nvptx.cc)
target_link_libraries(tlang-device-nvptx pthread CUDA::cudart)