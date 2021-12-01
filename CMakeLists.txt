cmake_minimum_required(VERSION 3.10)

project(tlang VERSION 0.42 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common) 
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ast) 
include(lexer/CMakeLists.txt) 
include(parser/CMakeLists.txt)
#link_directories(parser/)
add_executable(tlang driver/driver.cc)
target_link_libraries(tlang lexer parser ${FLEX_LIBRARIES})