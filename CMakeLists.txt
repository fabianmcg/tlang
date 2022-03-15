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
include(lib/CodeGen/CMakeLists.txt)
include(lib/AST/CMakeLists.txt)
include(lib/Driver/CMakeLists.txt)

add_executable(tlang lib/Driver/main.cxx)

target_include_directories(tlang PRIVATE ${LLVM_INC})
target_link_directories(tlang PRIVATE ${LLVM_LIB})

find_package(Boost 1.70.0 COMPONENTS program_options) 
if(Boost_FOUND)
	target_include_directories(tlang PRIVATE ${Boost_INCLUDE_DIRS}) 
	target_link_libraries(tlang PRIVATE ${Boost_LIBRARIES})
endif()

target_link_libraries(tlang PRIVATE driver lexer parser ast codegen ${FLEX_LIBRARIES} LLVM-13)

