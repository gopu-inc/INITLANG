# src/core/parser/CMakeLists.txt
add_library(initlang_parser
    parser.h
)

target_include_directories(initlang_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(initlang_parser initlang_lexer)
