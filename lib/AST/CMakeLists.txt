include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(ast STATIC
	${CMAKE_CURRENT_LIST_DIR}/DeclContext.cc
)
link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(ast)