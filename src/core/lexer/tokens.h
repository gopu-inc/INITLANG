// src/core/lexer/tokens.h
#pragma once
#include <string>
#include <unordered_map>

namespace initlang {
namespace lexer {

enum class TokenType {
    // Identifiants & Littéraux
    IDENTIFIER, NUMBER, STRING,
    
    // Mots-clés
    LET, FI, INIT, CONST, ASYNC, SPAWN, AWAIT,
    STRUCT, ENUM, MATCH, RETURN, IF, ELSE, WHILE, FOR,
    
    // Opérateurs
    ARROW,        // ==>
    DOUBLE_ARROW, // => 
    DOT,          // .
    COLON,        // :
    COMMA,        // ,
    SEMICOLON,    // ;
    
    // Parenthèses & délimiteurs
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    
    // Opérateurs mathématiques
    PLUS, MINUS, STAR, SLASH, PERCENT,
    
    // Comparaisons
    EQ, NEQ, LT, GT, LTE, GTE,
    ASSIGN,        // =
    
    // Logique
    AND, OR, NOT,
    
    // Spéciaux INITLANG
    INIT_LOG,     // init.log
    INIT_GER,     // init.ger
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l = 1, int c = 1) 
        : type(t), value(v), line(l), column(c) {}
};

class TokenUtils {
public:
    static std::string tokenTypeToString(TokenType type) {
        static const std::unordered_map<TokenType, std::string> typeNames = {
            {TokenType::IDENTIFIER, "IDENTIFIER"},
            {TokenType::NUMBER, "NUMBER"},
            {TokenType::STRING, "STRING"},
            {TokenType::LET, "LET"},
            {TokenType::FI, "FI"},
            {TokenType::ARROW, "ARROW"},
            {TokenType::INIT_GER, "INIT_GER"},
            {TokenType::INIT_LOG, "INIT_LOG"},
            {TokenType::EOF_TOKEN, "EOF"}
        };
        
        auto it = typeNames.find(type);
        return it != typeNames.end() ? it->second : "UNKNOWN";
    }
};

} // namespace lexer
} // namespace initlang
