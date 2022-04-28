cmake_minimum_required(VERSION 3.10)

project(tlang-tblgen VERSION 0.42 LANGUAGES CXX)

option(BUILD_FMT "Build libfmt" OFF)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(LLVM REQUIRED CONFIG)

if(BUILD_FMT)
  include(FetchContent)
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(fmt)
else()
  find_package(fmt REQUIRED)
endif()

file(GLOB_RECURSE SOURCES_LIST lib/**.cc)

add_executable(tlang-tblgen driver.cc)

target_include_directories(tlang-tblgen PRIVATE ${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include) 
target_link_directories(tlang-tblgen PRIVATE ${LLVM_LIBRARY_DIR})
target_sources(tlang-tblgen PRIVATE ${SOURCES_LIST})
target_link_libraries(tlang-tblgen LLVM LLVMTableGen fmt::fmt)
