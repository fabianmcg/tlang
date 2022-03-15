include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(driver STATIC
	${CMAKE_CURRENT_LIST_DIR}/Driver.cc
)
link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)
target_include_directories(driver PRIVATE ${LLVM_INC})
find_package(Boost 1.70.0 COMPONENTS program_options) 
if(Boost_FOUND)
	target_include_directories(driver PRIVATE ${Boost_INCLUDE_DIRS}) 
	target_link_libraries(driver PRIVATE ${Boost_LIBRARIES})
endif()
target_link_libraries(driver)