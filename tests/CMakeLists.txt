# tests/CMakeLists.txt
add_executable(test_core test_core.cpp)
target_link_libraries(test_core initlang_lexer initlang_parser)

# Compilation principale
add_executable(initlang_main ../src/frontend/cli/main.cpp)
target_link_libraries(initlang_main initlang_lexer initlang_parser)
