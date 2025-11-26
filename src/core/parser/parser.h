// src/core/parser/parser.h
#pragma once
#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include <memory>
#include <vector>
#include <stdexcept>

namespace initlang {
namespace parser {

class Parser {
private:
    lexer::Lexer& lexer;
    lexer::Token current_token;
    lexer::Token peek_token;
    
    void next_token() {
        current_token = peek_token;
        peek_token = lexer.next_token();
    }
    
    bool current_token_is(lexer::TokenType type) {
        return current_token.type == type;
    }
    
    bool peek_token_is(lexer::TokenType type) {
        return peek_token.type == type;
    }
    
    bool expect_peek(lexer::TokenType type) {
        if (peek_token_is(type)) {
            next_token();
            return true;
        }
        return false;
    }
    
    void error(const std::string& msg) {
        throw std::runtime_error(msg + " at line " + 
                                std::to_string(current_token.line) + ":" + 
                                std::to_string(current_token.column));
    }

public:
    Parser(lexer::Lexer& l) : lexer(l) {
        next_token();
        next_token();
    }
    
    std::unique_ptr<ast::Program> parse_program() {
        auto program = std::make_unique<ast::Program>();
        
        while (!current_token_is(lexer::TokenType::EOF_TOKEN)) {
            auto stmt = parse_statement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
            next_token();
        }
        
        return program;
    }
    
private:
    std::unique_ptr<ast::Statement> parse_statement() {
        switch (current_token.type) {
            case lexer::TokenType::LET:
                return parse_let_statement();
            case lexer::TokenType::FI:
                return parse_function_statement();
            case lexer::TokenType::RETURN:
                return parse_return_statement();
            default:
                return parse_expression_statement();
        }
    }
    
    std::unique_ptr<ast::VariableDeclaration> parse_let_statement() {
        // let x ==> 5
        next_token(); // skip 'let'
        
        if (!current_token_is(lexer::TokenType::IDENTIFIER)) {
            error("Expected identifier after 'let'");
            return nullptr;
        }
        
        std::string name = current_token.value;
        next_token(); // skip identifier
        
        if (!expect_peek(lexer::TokenType::ARROW)) {
            error("Expected '==>' after variable name");
            return nullptr;
        }
        
        next_token(); // skip '==>'
        auto value = parse_expression(LOWEST);
        
        return std::make_unique<ast::VariableDeclaration>(name, std::move(value));
    }
    
    std::unique_ptr<ast::FunctionDeclaration> parse_function_statement() {
        // fi add(a, b) { return a + b }
        next_token(); // skip 'fi'
        
        if (!current_token_is(lexer::TokenType::IDENTIFIER)) {
            error("Expected function name after 'fi'");
            return nullptr;
        }
        
        std::string name = current_token.value;
        next_token(); // skip function name
        
        if (!expect_peek(lexer::TokenType::LPAREN)) {
            error("Expected '(' after function name");
            return nullptr;
        }
        
        auto params = parse_function_parameters();
        
        if (!expect_peek(lexer::TokenType::LBRACE)) {
            error("Expected '{' after function parameters");
            return nullptr;
        }
        
        auto body = parse_block_statement();
        
        return std::make_unique<ast::FunctionDeclaration>(name, std::move(params), std::move(body));
    }
    
    std::vector<std::string> parse_function_parameters() {
        std::vector<std::string> parameters;
        
        if (peek_token_is(lexer::TokenType::RPAREN)) {
            next_token();
            return parameters;
        }
        
        next_token(); // skip '(' or ','
        
        if (current_token_is(lexer::TokenType::IDENTIFIER)) {
            parameters.push_back(current_token.value);
        } else {
            error("Expected parameter name");
            return parameters;
        }
        
        while (peek_token_is(lexer::TokenType::COMMA)) {
            next_token(); // skip identifier
            next_token(); // skip ','
            
            if (current_token_is(lexer::TokenType::IDENTIFIER)) {
                parameters.push_back(current_token.value);
            } else {
                error("Expected parameter name after ','");
                break;
            }
        }
        
        if (!expect_peek(lexer::TokenType::RPAREN)) {
            error("Expected ')' after parameters");
        }
        
        return parameters;
    }
    
    std::unique_ptr<ast::BlockStatement> parse_block_statement() {
        auto block = std::make_unique<ast::BlockStatement>();
        next_token(); // skip '{'
        
        while (!current_token_is(lexer::TokenType::RBRACE) && 
               !current_token_is(lexer::TokenType::EOF_TOKEN)) {
            auto stmt = parse_statement();
            if (stmt) {
                block->statements.push_back(std::move(stmt));
            }
            next_token();
        }
        
        return block;
    }
    
    std::unique_ptr<ast::ReturnStatement> parse_return_statement() {
        next_token(); // skip 'return'
        auto value = parse_expression(LOWEST);
        return std::make_unique<ast::ReturnStatement>(std::move(value));
    }
    
    std::unique_ptr<ast::ExpressionStatement> parse_expression_statement() {
        auto expr = parse_expression(LOWEST);
        return std::make_unique<ast::ExpressionStatement>(std::move(expr));
    }
    
    // Précedence des opérateurs
    enum Precedence {
        LOWEST,
        EQUALS,      // ==
        LESSGREATER, // > or <
        SUM,         // +
        PRODUCT,     // *
        PREFIX,      // -X or !X
        CALL         // myFunction(X)
    };
    
    std::unordered_map<lexer::TokenType, Precedence> precedences = {
        {lexer::TokenType::EQ, EQUALS},
        {lexer::TokenType::NEQ, EQUALS},
        {lexer::TokenType::LT, LESSGREATER},
        {lexer::TokenType::GT, LESSGREATER},
        {lexer::TokenType::LTE, LESSGREATER},
        {lexer::TokenType::GTE, LESSGREATER},
        {lexer::TokenType::PLUS, SUM},
        {lexer::TokenType::MINUS, SUM},
        {lexer::TokenType::SLASH, PRODUCT},
        {lexer::TokenType::STAR, PRODUCT},
        {lexer::TokenType::LPAREN, CALL},
    };
    
    Precedence current_precedence() {
        auto it = precedences.find(current_token.type);
        if (it != precedences.end()) {
            return it->second;
        }
        return LOWEST;
    }
    
    Precedence peek_precedence() {
        auto it = precedences.find(peek_token.type);
        if (it != precedences.end()) {
            return it->second;
        }
        return LOWEST;
    }
    
    std::unique_ptr<ast::Expression> parse_expression(Precedence precedence) {
        auto left = parse_prefix();
        if (!left) return nullptr;
        
        while (!peek_token_is(lexer::TokenType::SEMICOLON) && 
               precedence < peek_precedence()) {
            left = parse_infix(std::move(left));
            if (!left) return nullptr;
        }
        
        return left;
    }
    
    std::unique_ptr<ast::Expression> parse_prefix() {
        switch (current_token.type) {
            case lexer::TokenType::IDENTIFIER:
                return parse_identifier();
            case lexer::TokenType::NUMBER:
                return parse_number_literal();
            case lexer::TokenType::STRING:
                return parse_string_literal();
            case lexer::TokenType::INIT_GER:
                return parse_init_ger();
            case lexer::TokenType::LPAREN:
                return parse_grouped_expression();
            case lexer::TokenType::MINUS:
            case lexer::TokenType::NOT:
                return parse_prefix_expression();
            default:
                error("No prefix parse function for " + current_token.value);
                return nullptr;
        }
    }
    
    std::unique_ptr<ast::Expression> parse_infix(std::unique_ptr<ast::Expression> left) {
        switch (current_token.type) {
            case lexer::TokenType::PLUS:
            case lexer::TokenType::MINUS:
            case lexer::TokenType::STAR:
            case lexer::TokenType::SLASH:
            case lexer::TokenType::EQ:
            case lexer::TokenType::NEQ:
            case lexer::TokenType::LT:
            case lexer::TokenType::GT:
            case lexer::TokenType::LTE:
            case lexer::TokenType::GTE:
                return parse_binary_expression(std::move(left));
            case lexer::TokenType::LPAREN:
                return parse_call_expression(std::move(left));
            default:
                return left;
        }
    }
    
    std::unique_ptr<ast::Identifier> parse_identifier() {
        return std::make_unique<ast::Identifier>(current_token.value);
    }
    
    std::unique_ptr<ast::NumberLiteral> parse_number_literal() {
        try {
            double value = std::stod(current_token.value);
            return std::make_unique<ast::NumberLiteral>(value);
        } catch (...) {
            error("Could not parse number: " + current_token.value);
            return nullptr;
        }
    }
    
    std::unique_ptr<ast::StringLiteral> parse_string_literal() {
        return std::make_unique<ast::StringLiteral>(current_token.value);
    }
    
    std::unique_ptr<ast::Expression> parse_init_ger() {
        // init.ger(expression)
        next_token(); // skip 'init.ger'
        
        if (!expect_peek(lexer::TokenType::LPAREN)) {
            error("Expected '(' after init.ger");
            return nullptr;
        }
        
        next_token(); // skip '('
        auto arg = parse_expression(LOWEST);
        
        if (!expect_peek(lexer::TokenType::RPAREN)) {
            error("Expected ')' after init.ger argument");
            return nullptr;
        }
        
        // Pour l'instant, on retourne simplement l'argument
        // Plus tard, ce sera un CallExpression spécial
        return arg;
    }
    
    std::unique_ptr<ast::Expression> parse_grouped_expression() {
        next_token(); // skip '('
        auto expr = parse_expression(LOWEST);
        
        if (!expect_peek(lexer::TokenType::RPAREN)) {
            error("Expected ')' after expression");
            return nullptr;
        }
        
        return expr;
    }
    
    std::unique_ptr<ast::Expression> parse_prefix_expression() {
        auto op = current_token.type;
        next_token(); // skip operator
        auto right = parse_expression(PREFIX);
        
        if (!right) return nullptr;
        
        // Pour l'instant, on gère seulement la négation
        if (op == lexer::TokenType::MINUS) {
            // Créer une expression binaire: 0 - right
            auto zero = std::make_unique<ast::NumberLiteral>(0);
            return std::make_unique<ast::BinaryExpression>(
                lexer::TokenType::MINUS, std::move(zero), std::move(right));
        }
        
        return right;
    }
    
    std::unique_ptr<ast::BinaryExpression> parse_binary_expression(std::unique_ptr<ast::Expression> left) {
        auto op = current_token.type;
        auto precedence = current_precedence();
        
        next_token(); // skip operator
        auto right = parse_expression(precedence);
        
        if (!right) return nullptr;
        
        return std::make_unique<ast::BinaryExpression>(op, std::move(left), std::move(right));
    }
    
    std::unique_ptr<ast::CallExpression> parse_call_expression(std::unique_ptr<ast::Expression> function) {
        auto call_expr = std::make_unique<ast::CallExpression>();
        call_expr->callee = std::move(function);
        call_expr->arguments = parse_call_arguments();
        return call_expr;
    }
    
    std::vector<std::unique_ptr<ast::Expression>> parse_call_arguments() {
        std::vector<std::unique_ptr<ast::Expression>> args;
        
        if (peek_token_is(lexer::TokenType::RPAREN)) {
            next_token();
            return args;
        }
        
        next_token(); // skip '(' or ','
        args.push_back(parse_expression(LOWEST));
        
        while (peek_token_is(lexer::TokenType::COMMA)) {
            next_token(); // skip expression
            next_token(); // skip ','
            args.push_back(parse_expression(LOWEST));
        }
        
        if (!expect_peek(lexer::TokenType::RPAREN)) {
            error("Expected ')' after arguments");
        }
        
        return args;
    }
};

} // namespace parser
} // namespace initlang
