cmake_minimum_required(VERSION 3.10)

project(tlang VERSION 0.42 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(include/Lexer/CMakeLists.txt) 
include(lib/Parser/CMakeLists.txt)

add_executable(tlang lib/Driver/Driver.cc)

set(LLVM_INC /usr/lib/llvm-13/include)
set(LLVM_LIB /usr/lib/lib)

target_include_directories(tlang PRIVATE
    ${LLVM_INC}
)
target_link_directories(tlang PRIVATE
    ${LLVM_LIB}
)

target_link_libraries(tlang lexer parser ${FLEX_LIBRARIES} LLVM-13)

