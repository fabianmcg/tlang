#include_directories(${CMAKE_CURRENT_BINARY_DIR} 
#	${CMAKE_CURRENT_LIST_DIR}
#)

file(GLOB_RECURSE AST_SOURCES_LIST ${CMAKE_CURRENT_LIST_DIR}/*.cc)

add_library(ast STATIC
	${AST_SOURCES_LIST}
)

link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(ast PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_directories(ast PRIVATE ${LLVM_LIBRARY_DIR})
target_link_libraries(ast LLVM)