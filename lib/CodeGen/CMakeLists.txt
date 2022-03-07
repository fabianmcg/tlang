include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_library(codegen STATIC
	${CMAKE_CURRENT_LIST_DIR}/CodeGen.cc
	${CMAKE_CURRENT_LIST_DIR}/Emit.cc
	${CMAKE_CURRENT_LIST_DIR}/Type.cc
	${CMAKE_CURRENT_LIST_DIR}/Expr.cc
)
link_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)

set(LLVM_INC /usr/lib/llvm-13/include)
set(LLVM_LIB /usr/lib/lib)

target_include_directories(codegen PRIVATE
    ${LLVM_INC}
)
target_link_directories(codegen PRIVATE
    ${LLVM_LIB}
)

target_link_libraries(codegen LLVM-13)