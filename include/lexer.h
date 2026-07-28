#pragma once

#include <vector>
#include <string>
#include "token.h"

class Lexer
{
public:

    explicit Lexer(const std::string& source);

    std::vector<Token> scanTokens();

private:

    std::string source;

    std::vector<Token> tokens;

    size_t start = 0;
    size_t current = 0;

    int line = 1;

    bool isAtEnd();

    char advance();

    char peek();

    bool match(char expected);

    void scanToken();

    void addToken(TokenType type);

    void number();

    void identifier();
};