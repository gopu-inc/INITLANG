// src/core/ast/ast.h
#pragma once
#include "../lexer/tokens.h"
#include <memory>
#include <vector>
#include <string>

namespace initlang {
namespace ast {

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expressions
class Expression : public ASTNode {};

class NumberLiteral : public Expression {
public:
    double value;
    NumberLiteral(double val) : value(val) {}
};

class StringLiteral : public Expression {
public:
    std::string value;
    StringLiteral(const std::string& val) : value(val) {}
};

class Identifier : public Expression {
public:
    std::string name;
    Identifier(const std::string& n) : name(n) {}
};

class BinaryExpression : public Expression {
public:
    lexer::TokenType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    
    BinaryExpression(lexer::TokenType operation, 
                    std::unique_ptr<Expression> l, 
                    std::unique_ptr<Expression> r)
        : op(operation), left(std::move(l)), right(std::move(r)) {}
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    CallExpression(std::unique_ptr<Expression> c, 
                  std::vector<std::unique_ptr<Expression>> args)
        : callee(std::move(c)), arguments(std::move(args)) {}
};

// Statements
class Statement : public ASTNode {};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ExpressionStatement(std::unique_ptr<Expression> expr) 
        : expression(std::move(expr)) {}
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> value;
    bool is_const;
    
    VariableDeclaration(const std::string& n, std::unique_ptr<Expression> v, bool is_const = false)
        : name(n), value(std::move(v)), is_const(is_const) {}
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStatement> body;
    
    FunctionDeclaration(const std::string& n, 
                       std::vector<std::string> params,
                       std::unique_ptr<BlockStatement> b)
        : name(n), parameters(std::move(params)), body(std::move(b)) {}
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    BlockStatement() = default;
    BlockStatement(std::vector<std::unique_ptr<Statement>> stmts)
        : statements(std::move(stmts)) {}
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> value;
    
    ReturnStatement(std::unique_ptr<Expression> val = nullptr) 
        : value(std::move(val)) {}
};

// Programme
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace ast
} // namespace initlang
