#include "lexer.h"

#include <cctype>
#include <iostream>

std::unordered_map<std::string, TokenType> Lexer::keywords = {
	{"let", TokenType::LET},
	{"print", TokenType::PRINT},
	{"if", TokenType::IF},
	{"else", TokenType::ELSE},
	{"while", TokenType::WHILE},
	{"true", TokenType::TRUE},
	{"false", TokenType::FALSE},
};

Lexer::Lexer(const std::string& source)
	: source(source)
{
}

std::vector<Token> Lexer::scanTokens()
{
	while(!isAtEnd())
	{
		start = current;
		scanToken();
	}

	tokens.emplace_back(TokenType::END_OF_FILE, "", line);
	return tokens;
}

bool Lexer::isAtEnd()
{
	return current >= source.size();
}

char Lexer::advance()
{
	return source[current++];
}

char Lexer::peek()
{
	if(isAtEnd())
	{
		return '\0';
	}

	return source[current];
}

char Lexer::peekNext()
{
	if(current + 1 >= source.size())
	{
		return '\0';
	}

	return source[current + 1];
}

bool Lexer::match(char expected)
{
	if(isAtEnd() || source[current] != expected)
	{
		return false;
	}

	++current;
	return true;
}

void Lexer::addToken(TokenType type)
{
	tokens.emplace_back(type, source.substr(start, current - start), line);
}

void Lexer::scanToken()
{
	char c = advance();

	switch(c)
	{
		case '(': addToken(TokenType::LEFT_PAREN); break;
		case ')': addToken(TokenType::RIGHT_PAREN); break;
		case '{': addToken(TokenType::LEFT_BRACE); break;
		case '}': addToken(TokenType::RIGHT_BRACE); break;
		case ',': addToken(TokenType::COMMA); break;
		case '.': addToken(TokenType::DOT); break;
		case '-': addToken(TokenType::MINUS); break;
		case '+': addToken(TokenType::PLUS); break;
		case ';': addToken(TokenType::SEMICOLON); break;
		case '*': addToken(TokenType::STAR); break;
		case '%': addToken(TokenType::MOD); break;

		case '!':
			addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
			break;

		case '=':
			addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
			break;

		case '<':
			addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
			break;

		case '>':
			addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
			break;

		case '/':
			if(match('/'))
			{
				while(peek() != '\n' && !isAtEnd())
				{
					advance();
				}
			}
			else
			{
				addToken(TokenType::SLASH);
			}
			break;

		case ' ':
		case '\r':
		case '\t':
			break;

		case '\n':
			++line;
			break;

		case '"':
			string();
			break;

		default:
			if(std::isdigit(static_cast<unsigned char>(c)))
			{
				number();
			}
			else if(std::isalpha(static_cast<unsigned char>(c)) || c == '_')
			{
				identifier();
			}
			else
			{
				// NOTE: A proper error reporting mechanism would be better.
				std::cerr << "Line " << line << ": Unexpected character '" << c << "'.\n";
			}
			break;
	}
}

void Lexer::number()
{
	while(std::isdigit(static_cast<unsigned char>(peek())))
	{
		advance();
	}

	if(peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext())))
	{
		advance();

		while(std::isdigit(static_cast<unsigned char>(peek())))
		{
			advance();
		}
	}

	addToken(TokenType::NUMBER);
}

void Lexer::identifier()
{
	while(std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
	{
		advance();
	}

	const std::string text = source.substr(start, current - start);
	const auto keyword = keywords.find(text);

	if(keyword != keywords.end())
	{
		tokens.emplace_back(keyword->second, text, line);
		return;
	}

	tokens.emplace_back(TokenType::IDENTIFIER, text, line);
}

void Lexer::string()
{
	while(peek() != '"' && !isAtEnd())
	{
		if(peek() == '\n')
		{
			++line;
		}

		advance();
	}

	if(isAtEnd())
	{
		// NOTE: A proper error reporting mechanism would be better.
		std::cerr << "Line " << line << ": Unterminated string.\n";
		return;
	}

	// The closing ".
	advance();

	// Trim the surrounding quotes.
	// The +1 for start skips the opening ", and -2 for length accounts for both quotes.
	const std::string value = source.substr(start + 1, current - start - 2);
	tokens.emplace_back(TokenType::STRING, value, line);
}
