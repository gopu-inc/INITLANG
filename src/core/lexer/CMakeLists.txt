# src/core/lexer/CMakeLists.txt
add_library(initlang_lexer
    tokens.h
    lexer.h
)

target_include_directories(initlang_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
