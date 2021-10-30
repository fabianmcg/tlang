include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(driver STATIC
	${CMAKE_CURRENT_LIST_DIR}/driver.cc
)