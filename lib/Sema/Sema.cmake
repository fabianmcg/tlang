file(GLOB_RECURSE SEMA_SOURCES_LIST ${CMAKE_CURRENT_LIST_DIR}/*.cc)
add_lib(sema "${SEMA_SOURCES_LIST}" #[[USE_LLVM]] ON #[[USE_BOOST]] OFF #[[USE_FRMT]] OFF "")