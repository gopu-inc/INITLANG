// src/core/lexer/tokens.h (EXTRAIT CORRIGÉ)
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    // Constructeur par défaut
    Token() : type(TokenType::EOF_TOKEN), value(""), line(1), column(1) {}
    
    Token(TokenType t, const std::string& v, int l = 1, int c = 1) 
        : type(t), value(v), line(l), column(c) {}
};
