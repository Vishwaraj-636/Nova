#pragma once

#include <string>

enum class TokenType
{
    // literals
    NUMBER,
    IDENTIFIER,

    // keywords
    LET,
    PRINT,
    IF,
    ELSE,
    WHILE,

    // operators
    PLUS,
    MINUS,
    STAR,
    SLASH,

    ASSIGN,

    EQUAL,
    NOT_EQUAL,

    LESS,
    LESS_EQUAL,

    GREATER,
    GREATER_EQUAL,

    // punctuation
    LPAREN,
    RPAREN,

    LBRACE,
    RBRACE,

    SEMICOLON,

    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;

    Token(
        TokenType type,
        std::string lexeme,
        int line)
        :
        type(type),
        lexeme(std::move(lexeme)),
        line(line)
    {
    }
};