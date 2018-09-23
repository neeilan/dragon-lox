#ifndef LOXPP_SRC_TOKEN_H_
#define LOXPP_SRC_TOKEN_H_

#include <string>

enum TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER,

    // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, LAMBDA, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE, END_OF_FILE
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;

    Token(TokenType type,
          const std::string& lexeme,
          const std::string& literal,
          int line)
            : type(type),
              lexeme(lexeme),
              literal(literal),
              line(line) {}

    Token(const Token& token) {
        this->type = token.type;
        this->lexeme = token.lexeme;
        this->literal = token.literal;
        this->line = token.line;
    }

    std::string str() const;
};

#endif

