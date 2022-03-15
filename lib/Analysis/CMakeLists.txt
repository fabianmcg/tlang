include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(analysis STATIC
	${CMAKE_CURRENT_LIST_DIR}/Analysis.cc
)
link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(analysis)