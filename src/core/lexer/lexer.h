// src/core/lexer/lexer.h
class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;
    
    char current_char();
    void advance();
    void skip_whitespace();
    Token read_identifier();
    Token read_number();
    Token read_string();
    
public:
    Lexer(const std::string& source);
    Token next_token();
    std::vector<Token> tokenize();
};
