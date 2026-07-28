#pragma once

#include <string>

enum class TokenType
{
    // Single-character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,
    MOD,

    // One or two character tokens
    BANG,
    BANG_EQUAL,

    EQUAL,
    EQUAL_EQUAL,

    GREATER,
    GREATER_EQUAL,

    LESS,
    LESS_EQUAL,

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords
    LET,
    PRINT,
    IF,
    ELSE,
    WHILE,
    TRUE,
    FALSE,

    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;

    Token(
        TokenType t,
        const std::string& l,
        int ln)
        :
        type(t),
        lexeme(l),
        line(ln)
    {}
};