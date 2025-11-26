// src/core/ast/ast.h
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Expressions
class Expression : public ASTNode {};

class NumberLiteral : public Expression {
public:
    double value;
};

class StringLiteral : public Expression {
public:
    std::string value;
};

class Identifier : public Expression {
public:
    std::string name;
};

class BinaryExpression : public Expression {
public:
    TokenType operator;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

// Statements
class Statement : public ASTNode {};

class VariableDeclaration : public Statement {
public:
    std::string name;
    std::unique_ptr<Expression> value;
    bool is_const;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<BlockStatement> body;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};
