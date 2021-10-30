cmake_minimum_required(VERSION 3.10)

project(ttc VERSION 0.42 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common) 
include(lexer/CMakeLists.txt)
#include(driver/CMakeLists.txt)

add_executable(ttc driver/driver.cc)
target_link_libraries(ttc lexer ${FLEX_LIBRARIES})