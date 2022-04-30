find_package(FLEX REQUIRED)
FLEX_TARGET(lex ${CMAKE_CURRENT_LIST_DIR}/lex.yy ${CMAKE_CURRENT_BINARY_DIR}/lex.cc COMPILE_FLAGS "-+")
add_library(lex STATIC
	lex.cc
    ${FLEX_lexer_OUTPUTS}
)
target_include_directories(lex PRIVATE ${CMAKE_CURRENT_LIST_DIR})
#target_link_libraries(lex ${FLEX_LIBRARIES})
