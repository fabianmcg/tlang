include_directories(
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(parser STATIC
	${CMAKE_CURRENT_LIST_DIR}/parser.cc
)
link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(parser)