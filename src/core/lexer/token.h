// src/core/lexer/tokens.h
enum class TokenType {
    // Identifiants & Littéraux
    IDENTIFIER, NUMBER, STRING,
    
    // Mots-clés
    LET, FI, INIT, CONST, ASYNC, SPAWN, AWAIT,
    STRUCT, ENUM, MATCH, RETURN,
    
    // Opérateurs
    ARROW,        // ==>
    DOT,          // .
    COLON,        // :
    COMMA,        // ,
    
    // Parenthèses & délimiteurs
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    
    // Opérateurs mathématiques
    PLUS, MINUS, STAR, SLASH, PERCENT,
    
    // Comparaisons
    EQ, NEQ, LT, GT, LTE, GTE,
    
    // Spéciaux
    INIT_LOG,     // init.log
    INIT_GER,     // init.ger
    EOF
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};
