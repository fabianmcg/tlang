find_package(FLEX REQUIRED)
FLEX_TARGET(lexer ${CMAKE_CURRENT_LIST_DIR}/lex.yy ${CMAKE_CURRENT_BINARY_DIR}/lex.cc COMPILE_FLAGS "-+")
include_directories(${CMAKE_CURRENT_BINARY_DIR} 
	${CMAKE_CURRENT_LIST_DIR}
)
add_custom_command(
  SOURCE ${CMAKE_CURRENT_LIST_DIR}/toks.yml
  OUTPUT lex.yy
  COMMAND python3 tblgen.py
  VERBATIM)
add_library(lexer STATIC
	lex.cc
    ${FLEX_MyScanner_OUTPUTS}
)
target_link_libraries(lexer ${FLEX_LIBRARIES})
