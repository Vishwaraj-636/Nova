#include "interpreter.h"

#include <cmath>
#include <iostream>

bool Interpreter::execute(const std::vector<Token>& inputTokens)
{
    tokens = &inputTokens;
    current = 0;
    hadError = false;
    environment.clear();

    while(!isAtEnd())
    {
        if(!declaration())
        {
            synchronize();
        }
    }

    return !hadError;
}

bool Interpreter::isAtEnd() const
{
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Interpreter::peek() const
{
    return (*tokens)[current];
}

const Token& Interpreter::previous() const
{
    return (*tokens)[current - 1];
}

const Token& Interpreter::advance()
{
    if(!isAtEnd())
    {
        ++current;
    }

    return previous();
}

bool Interpreter::check(TokenType type) const
{
    if(isAtEnd())
    {
        return false;
    }

    return peek().type == type;
}

bool Interpreter::match(std::initializer_list<TokenType> types)
{
    for(const auto type : types)
    {
        if(check(type))
        {
            advance();
            return true;
        }
    }

    return false;
}

const Token& Interpreter::consume(TokenType type, const std::string& message)
{
    if(check(type))
    {
        return advance();
    }

    std::cerr << "Line " << peek().line << ": " << message << "\n";
    hadError = true;
    return peek();
}

void Interpreter::synchronize()
{
    advance();

    while(!isAtEnd())
    {
        if(previous().type == TokenType::SEMICOLON)
        {
            return;
        }

        switch(peek().type)
        {
            case TokenType::LET:
            case TokenType::PRINT:
            case TokenType::IF:
            case TokenType::WHILE:
                return;

            default:
                break;
        }

        advance();
    }
}

bool Interpreter::declaration()
{
    if(match({TokenType::LET}))
    {
        return letDeclaration();
    }

    return statement();
}

bool Interpreter::statement()
{
    if(match({TokenType::PRINT}))
    {
        return printStatement();
    }

    if(match({TokenType::IF}))
    {
        return ifStatement();
    }

    if(match({TokenType::WHILE}))
    {
        return whileStatement();
    }

    if(match({TokenType::LEFT_BRACE}))
    {
        return block();
    }

    return expressionStatement();
}

bool Interpreter::letDeclaration()
{
    const Token& name = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");
    consume(TokenType::EQUAL, "Expected '=' after variable name.");

    Value value;
    if(!expression(value))
    {
        return false;
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    environment[name.lexeme] = value;
    return true;
}

bool Interpreter::printStatement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'print'.");

    Value value;
    if(!expression(value))
    {
        return false;
    }

    consume(TokenType::RIGHT_PAREN, "Expected ')' after print value.");
    consume(TokenType::SEMICOLON, "Expected ';' after print statement.");

    std::cout << toString(value) << "\n";
    return true;
}

bool Interpreter::ifStatement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");

    Value condition;
    if(!expression(condition))
    {
        return false;
    }

    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");
    consume(TokenType::LEFT_BRACE, "Expected '{' to start if block.");

    const size_t thenLeftBrace = previous().type == TokenType::LEFT_BRACE ? current - 1 : current;
    const size_t thenBodyStart = current;
    const size_t thenBodyEnd = findMatchingRightBrace(thenLeftBrace);

    if(isTruthy(condition))
    {
        if(!executeRange(thenBodyStart, thenBodyEnd))
        {
            return false;
        }
    }

    current = thenBodyEnd + 1;

    if(match({TokenType::ELSE}))
    {
        consume(TokenType::LEFT_BRACE, "Expected '{' to start else block.");

        const size_t elseLeftBrace = previous().type == TokenType::LEFT_BRACE ? current - 1 : current;
        const size_t elseBodyStart = current;
        const size_t elseBodyEnd = findMatchingRightBrace(elseLeftBrace);

        if(!isTruthy(condition))
        {
            if(!executeRange(elseBodyStart, elseBodyEnd))
            {
                return false;
            }
        }

        current = elseBodyEnd + 1;
    }

    return true;
}

bool Interpreter::whileStatement()
{
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");

    const size_t conditionStart = current;
    Value condition;
    if(!expression(condition))
    {
        return false;
    }

    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition.");
    consume(TokenType::LEFT_BRACE, "Expected '{' to start while block.");

    const size_t bodyLeftBrace = previous().type == TokenType::LEFT_BRACE ? current - 1 : current;
    const size_t bodyStart = current;
    const size_t bodyEnd = findMatchingRightBrace(bodyLeftBrace);

    while(isTruthy(condition))
    {
        if(!executeRange(bodyStart, bodyEnd))
        {
            return false;
        }

        current = conditionStart;
        if(!expression(condition))
        {
            return false;
        }

        consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition.");
    }

    current = bodyEnd + 1;
    return true;
}

bool Interpreter::block()
{
    while(!isAtEnd() && !check(TokenType::RIGHT_BRACE))
    {
        if(!declaration())
        {
            synchronize();
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
    return true;
}

bool Interpreter::expressionStatement()
{
    Value value;
    if(!expression(value))
    {
        return false;
    }

    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return true;
}

bool Interpreter::expression(Value& out)
{
    return assignment(out);
}

bool Interpreter::assignment(Value& out)
{
    if(check(TokenType::IDENTIFIER) && current + 1 < tokens->size() && (*tokens)[current + 1].type == TokenType::EQUAL)
    {
        const Token name = advance();
        advance();

        Value value;
        if(!assignment(value))
        {
            return false;
        }

        environment[name.lexeme] = value;
        out = value;
        return true;
    }

    return equality(out);
}

bool Interpreter::equality(Value& out)
{
    if(!comparison(out))
    {
        return false;
    }

    while(match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
        const Token op = previous();
        Value right;
        if(!comparison(right))
        {
            return false;
        }

        out = Value::booleanValue(isEqual(out, right) == (op.type == TokenType::EQUAL_EQUAL));
    }

    return true;
}

bool Interpreter::comparison(Value& out)
{
    if(!term(out))
    {
        return false;
    }

    while(match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
    {
        const Token op = previous();
        Value right;
        if(!term(right))
        {
            return false;
        }

        out = applyBinary(op, out, right);
    }

    return true;
}

bool Interpreter::term(Value& out)
{
    if(!factor(out))
    {
        return false;
    }

    while(match({TokenType::PLUS, TokenType::MINUS}))
    {
        const Token op = previous();
        Value right;
        if(!factor(right))
        {
            return false;
        }

        out = applyBinary(op, out, right);
    }

    return true;
}

bool Interpreter::factor(Value& out)
{
    if(!unary(out))
    {
        return false;
    }

    while(match({TokenType::STAR, TokenType::SLASH, TokenType::MOD}))
    {
        const Token op = previous();
        Value right;
        if(!unary(right))
        {
            return false;
        }

        out = applyBinary(op, out, right);
    }

    return true;
}

bool Interpreter::unary(Value& out)
{
    if(match({TokenType::BANG}))
    {
        if(!unary(out))
        {
            return false;
        }

        out = Value::booleanValue(!isTruthy(out));
        return true;
    }

    if(match({TokenType::MINUS}))
    {
        if(!unary(out))
        {
            return false;
        }

        if(!isNumber(out))
        {
            std::cerr << "Line " << previous().line << ": Operand must be numeric.\n";
            hadError = true;
            return false;
        }

        out = Value::numberValue(-toNumber(out));
        return true;
    }

    return primary(out);
}

bool Interpreter::primary(Value& out)
{
    if(match({TokenType::FALSE}))
    {
        out = Value::booleanValue(false);
        return true;
    }

    if(match({TokenType::TRUE}))
    {
        out = Value::booleanValue(true);
        return true;
    }

    if(match({TokenType::NUMBER}))
    {
        out = Value::numberValue(std::stod(previous().lexeme));
        return true;
    }

    if(match({TokenType::STRING}))
    {
        out = Value::stringValue(previous().lexeme);
        return true;
    }

    if(match({TokenType::IDENTIFIER}))
    {
        const auto found = environment.find(previous().lexeme);
        if(found == environment.end())
        {
            std::cerr << "Line " << previous().line << ": Undefined variable '" << previous().lexeme << "'.\n";
            hadError = true;
            return false;
        }

        out = found->second;
        return true;
    }

    if(match({TokenType::LEFT_PAREN}))
    {
        if(!expression(out))
        {
            return false;
        }

        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return true;
    }

    std::cerr << "Line " << peek().line << ": Expected expression.\n";
    hadError = true;
    return false;
}

bool Interpreter::isTruthy(const Value& value) const
{
    switch(value.type)
    {
        case Value::Type::Nil:
            return false;

        case Value::Type::Number:
            return value.number != 0.0;

        case Value::Type::String:
            return !value.text.empty();

        case Value::Type::Boolean:
            return value.boolean;
    }

    return false;
}

bool Interpreter::isNumber(const Value& value) const
{
    return value.type == Value::Type::Number;
}

double Interpreter::toNumber(const Value& value) const
{
    return value.number;
}

std::string Interpreter::toString(const Value& value) const
{
    switch(value.type)
    {
        case Value::Type::Number:
        {
            std::string text = std::to_string(value.number);
            while(!text.empty() && text.back() == '0')
            {
                text.pop_back();
            }

            if(!text.empty() && text.back() == '.')
            {
                text.pop_back();
            }

            return text.empty() ? "0" : text;
        }

        case Value::Type::String:
            return value.text;

        case Value::Type::Boolean:
            return value.boolean ? "true" : "false";

        case Value::Type::Nil:
        default:
            return "nil";
    }
}

bool Interpreter::isEqual(const Value& left, const Value& right) const
{
    if(left.type != right.type)
    {
        return false;
    }

    switch(left.type)
    {
        case Value::Type::Nil:
            return true;

        case Value::Type::Number:
            return left.number == right.number;

        case Value::Type::String:
            return left.text == right.text;

        case Value::Type::Boolean:
            return left.boolean == right.boolean;
    }

    return false;
}

Interpreter::Value Interpreter::applyBinary(const Token& op, const Value& left, const Value& right)
{
    const bool leftNumber = isNumber(left);
    const bool rightNumber = isNumber(right);

    switch(op.type)
    {
        case TokenType::PLUS:
            if(leftNumber && rightNumber)
            {
                return Value::numberValue(toNumber(left) + toNumber(right));
            }

            return Value::stringValue(toString(left) + toString(right));

        case TokenType::MINUS:
            if(leftNumber && rightNumber)
            {
                return Value::numberValue(toNumber(left) - toNumber(right));
            }
            break;

        case TokenType::STAR:
            if(leftNumber && rightNumber)
            {
                return Value::numberValue(toNumber(left) * toNumber(right));
            }
            break;

        case TokenType::SLASH:
            if(leftNumber && rightNumber)
            {
                return Value::numberValue(toNumber(left) / toNumber(right));
            }
            break;

        case TokenType::MOD:
            if(leftNumber && rightNumber)
            {
                return Value::numberValue(std::fmod(toNumber(left), toNumber(right)));
            }
            break;

        case TokenType::GREATER:
            if(leftNumber && rightNumber)
            {
                return Value::booleanValue(toNumber(left) > toNumber(right));
            }
            break;

        case TokenType::GREATER_EQUAL:
            if(leftNumber && rightNumber)
            {
                return Value::booleanValue(toNumber(left) >= toNumber(right));
            }
            break;

        case TokenType::LESS:
            if(leftNumber && rightNumber)
            {
                return Value::booleanValue(toNumber(left) < toNumber(right));
            }
            break;

        case TokenType::LESS_EQUAL:
            if(leftNumber && rightNumber)
            {
                return Value::booleanValue(toNumber(left) <= toNumber(right));
            }
            break;

        default:
            break;
    }

    std::cerr << "Line " << op.line << ": Invalid operands for operator '" << op.lexeme << "'.\n";
    hadError = true;
    return Value::nilValue();
}

size_t Interpreter::findMatchingRightBrace(size_t leftBraceIndex)
{
    int depth = 0;

    for(size_t index = leftBraceIndex; index < tokens->size(); ++index)
    {
        if((*tokens)[index].type == TokenType::LEFT_BRACE)
        {
            ++depth;
        }
        else if((*tokens)[index].type == TokenType::RIGHT_BRACE)
        {
            --depth;

            if(depth == 0)
            {
                return index;
            }
        }
    }

    std::cerr << "Line " << (*tokens)[leftBraceIndex].line << ": Unmatched '{'.\n";
    hadError = true;
    return tokens->size() - 1;
}

bool Interpreter::executeRange(size_t startIndex, size_t endIndex)
{
    const size_t savedCurrent = current;
    current = startIndex;

    while(current < endIndex && !hadError)
    {
        if(!declaration())
        {
            synchronize();
        }
    }

    current = savedCurrent;
    return !hadError;
}