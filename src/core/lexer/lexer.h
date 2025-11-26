// src/core/lexer/lexer.h
#pragma once
#include "tokens.h"
#include <string>
#include <vector>
#include <memory>
#include <cctype>
#include <stdexcept>

namespace initlang {
namespace lexer {

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    char current_char;
    
    void advance() {
        if (position < source.length()) {
            current_char = source[position++];
            column++;
        } else {
            current_char = '\0';
        }
    }
    
    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            if (current_char == '\n') {
                line++;
                column = 1;
            }
            advance();
        }
    }
    
    char peek() {
        if (position < source.length()) {
            return source[position];
        }
        return '\0';
    }
    
    Token read_identifier() {
        int start_line = line;
        int start_column = column - 1;
        std::string result;
        
        while (current_char != '\0' && 
               (std::isalnum(current_char) || current_char == '_' || current_char == '.')) {
            result += current_char;
            advance();
        }
        
        // Mots-clés spéciaux INITLANG
        if (result == "init.ger") return Token(TokenType::INIT_GER, result, start_line, start_column);
        if (result == "init.log") return Token(TokenType::INIT_LOG, result, start_line, start_column);
        
        // Mots-clés normaux
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"let", TokenType::LET},
            {"fi", TokenType::FI},
            {"const", TokenType::CONST},
            {"return", TokenType::RETURN},
            {"async", TokenType::ASYNC},
            {"spawn", TokenType::SPAWN},
            {"await", TokenType::AWAIT}
        };
        
        auto it = keywords.find(result);
        if (it != keywords.end()) {
            return Token(it->second, result, start_line, start_column);
        }
        
        return Token(TokenType::IDENTIFIER, result, start_line, start_column);
    }
    
    Token read_number() {
        int start_line = line;
        int start_column = column - 1;
        std::string result;
        bool has_dot = false;
        
        while (current_char != '\0' && 
               (std::isdigit(current_char) || current_char == '.')) {
            if (current_char == '.') {
                if (has_dot) break; // Deux points = erreur
                has_dot = true;
            }
            result += current_char;
            advance();
        }
        
        return Token(TokenType::NUMBER, result, start_line, start_column);
    }
    
    Token read_string() {
        int start_line = line;
        int start_column = column - 1;
        std::string result;
        char quote = current_char;
        
        advance(); // Skip opening quote
        
        while (current_char != '\0' && current_char != quote) {
            if (current_char == '\\') {
                advance(); // Skip backslash
                switch (current_char) {
                    case 'n': result += '\n'; break;
                    case 't': result += '\t'; break;
                    case 'r': result += '\r'; break;
                    default: result += current_char; break;
                }
            } else {
                result += current_char;
            }
            advance();
        }
        
        if (current_char != quote) {
            throw std::runtime_error("Unterminated string at line " + std::to_string(line));
        }
        
        advance(); // Skip closing quote
        return Token(TokenType::STRING, result, start_line, start_column);
    }

public:
    Lexer(const std::string& source) 
        : source(source), position(0), line(1), column(1), current_char('\0') {
        advance();
    }
    
    Token next_token() {
        skip_whitespace();
        
        if (current_char == '\0') {
            return Token(TokenType::EOF_TOKEN, "", line, column);
        }
        
        // Identifiants
        if (std::isalpha(current_char) || current_char == '_') {
            return read_identifier();
        }
        
        // Nombres
        if (std::isdigit(current_char)) {
            return read_number();
        }
        
        // Chaînes de caractères
        if (current_char == '"' || current_char == '\'') {
            return read_string();
        }
        
        // Opérateurs et symboles
        int current_line = line;
        int current_column = column;
        char ch = current_char;
        
        // Opérateur arrow ==>
        if (ch == '=' && peek() == '=' && source[position + 1] == '>') {
            advance(); // =
            advance(); // =
            advance(); // >
            return Token(TokenType::ARROW, "==>", current_line, current_column);
        }
        
        // Double arrow =>
        if (ch == '=' && peek() == '>') {
            advance(); // =
            advance(); // >
            return Token(TokenType::DOUBLE_ARROW, "=>", current_line, current_column);
        }
        
        // Autres opérateurs simples
        switch (ch) {
            case '+': advance(); return Token(TokenType::PLUS, "+", current_line, current_column);
            case '-': advance(); return Token(TokenType::MINUS, "-", current_line, current_column);
            case '*': advance(); return Token(TokenType::STAR, "*", current_line, current_column);
            case '/': advance(); return Token(TokenType::SLASH, "/", current_line, current_column);
            case '%': advance(); return Token(TokenType::PERCENT, "%", current_line, current_column);
            case '(': advance(); return Token(TokenType::LPAREN, "(", current_line, current_column);
            case ')': advance(); return Token(TokenType::RPAREN, ")", current_line, current_column);
            case '{': advance(); return Token(TokenType::LBRACE, "{", current_line, current_column);
            case '}': advance(); return Token(TokenType::RBRACE, "}", current_line, current_column);
            case '[': advance(); return Token(TokenType::LBRACKET, "[", current_line, current_column);
            case ']': advance(); return Token(TokenType::RBRACKET, "]", current_line, current_column);
            case ',': advance(); return Token(TokenType::COMMA, ",", current_line, current_column);
            case ';': advance(); return Token(TokenType::SEMICOLON, ";", current_line, current_column);
            case ':': advance(); return Token(TokenType::COLON, ":", current_line, current_column);
            case '.': advance(); return Token(TokenType::DOT, ".", current_line, current_column);
            case '=': 
                if (peek() == '=') {
                    advance(); advance();
                    return Token(TokenType::EQ, "==", current_line, current_column);
                }
                advance();
                return Token(TokenType::ASSIGN, "=", current_line, current_column);
            case '!':
                if (peek() == '=') {
                    advance(); advance();
                    return Token(TokenType::NEQ, "!=", current_line, current_column);
                }
                advance();
                return Token(TokenType::NOT, "!", current_line, current_column);
            case '<':
                if (peek() == '=') {
                    advance(); advance();
                    return Token(TokenType::LTE, "<=", current_line, current_column);
                }
                advance();
                return Token(TokenType::LT, "<", current_line, current_column);
            case '>':
                if (peek() == '=') {
                    advance(); advance();
                    return Token(TokenType::GTE, ">=", current_line, current_column);
                }
                advance();
                return Token(TokenType::GT, ">", current_line, current_column);
        }
        
        // Caractère inconnu
        std::string unknown(1, ch);
        advance();
        throw std::runtime_error("Unexpected character '" + unknown + "' at line " + 
                                std::to_string(current_line) + ":" + std::to_string(current_column));
    }
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token token = next_token();
        
        while (token.type != TokenType::EOF_TOKEN) {
            tokens.push_back(token);
            token = next_token();
        }
        
        tokens.push_back(token); // EOF
        return tokens;
    }
};

} // namespace lexer
} // namespace initlang
