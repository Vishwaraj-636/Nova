#pragma once

#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

#include "token.h"

class Interpreter
{
public:
    bool execute(const std::vector<Token>& tokens);

private:
    struct Value
    {
        enum class Type
        {
            Nil,
            Number,
            String,
            Boolean,
        };

        Type type = Type::Nil;
        double number = 0.0;
        std::string text;
        bool boolean = false;

        static Value numberValue(double value)
        {
            Value out;
            out.type = Type::Number;
            out.number = value;
            return out;
        }

        static Value stringValue(const std::string& value)
        {
            Value out;
            out.type = Type::String;
            out.text = value;
            return out;
        }

        static Value booleanValue(bool value)
        {
            Value out;
            out.type = Type::Boolean;
            out.boolean = value;
            return out;
        }

        static Value nilValue()
        {
            return Value();
        }
    };

    const std::vector<Token>* tokens = nullptr;
    size_t current = 0;
    bool hadError = false;
    std::unordered_map<std::string, Value> environment;

    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types);
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();

    bool declaration();
    bool statement();
    bool letDeclaration();
    bool printStatement();
    bool ifStatement();
    bool whileStatement();
    bool block();
    bool expressionStatement();

    bool expression(Value& out);
    bool assignment(Value& out);
    bool equality(Value& out);
    bool comparison(Value& out);
    bool term(Value& out);
    bool factor(Value& out);
    bool unary(Value& out);
    bool primary(Value& out);

    bool isTruthy(const Value& value) const;
    bool isNumber(const Value& value) const;
    double toNumber(const Value& value) const;
    std::string toString(const Value& value) const;
    bool isEqual(const Value& left, const Value& right) const;
    Value applyBinary(const Token& op, const Value& left, const Value& right);

    size_t findMatchingRightBrace(size_t leftBraceIndex);
    bool executeRange(size_t startIndex, size_t endIndex);
};