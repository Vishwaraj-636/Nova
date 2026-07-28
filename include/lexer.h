#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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

    char peekNext();

    bool match(char expected);

    void addToken(TokenType);

    void scanToken();

    void number();

    void identifier();

    void string();

    static std::unordered_map<std::string, TokenType> keywords;
};