// tests/test_core.cpp
#include <iostream>
#include "../src/core/lexer/lexer.h"
#include "../src/core/parser/parser.h"

using namespace initlang;

void test_lexer() {
    std::cout << "=== TEST LEXER ===" << std::endl;
    
    std::string code = R"(
        let x ==> 5
        let name ==> "Mauricio"
        init.ger("Hello INITLANG!")
        fi add(a, b) {
            return a + b
        }
    )";
    
    lexer::Lexer lexer(code);
    
    try {
        auto tokens = lexer.tokenize();
        
        for (const auto& token : tokens) {
            std::cout << "Token: " << lexer::TokenUtils::tokenTypeToString(token.type) 
                      << " Value: '" << token.value << "'"
                      << " Line: " << token.line << ":" << token.column << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void test_parser() {
    std::cout << "\n=== TEST PARSER ===" << std::endl;
    
    std::string code = R"(
        let x ==> 5
        let result ==> x + 10 * 2
        init.ger("Calcul: " + result)
    )";
    
    lexer::Lexer lexer(code);
    parser::Parser parser(lexer);
    
    try {
        auto program = parser.parse_program();
        std::cout << "Program parsed successfully!" << std::endl;
        std::cout << "Number of statements: " << program->statements.size() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Parser Error: " << e.what() << std::endl;
    }
}

int main() {
    test_lexer();
    test_parser();
    return 0;
}
