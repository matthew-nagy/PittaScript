#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <stdlib.h>
#include "PittaValue.hpp"

namespace pitta {

	//Forwards declare the runtime
	class Runtime;

	enum TokenType {
		// Single-character tokens.
		LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
		COMMA, DOT, SEMICOLON,
		MINUS, SLASH, STAR, PERCENT,
		BIT_OR, BIT_AND, BIT_XOR, BIT_NOT,

		// One or two character tokens.
		BANG, BANG_EQUAL,
		EQUAL, EQUAL_EQUAL,
		GREATER, GREATER_EQUAL, SHIFT_RIGHT,
		LESS, LESS_EQUAL, SHIFT_LEFT,
		PLUS, STRING_CONCAT,

		// Literals.
		IDENTIFIER, STRING, INT, FLOAT,

		// Keywords.
		AND, BREAK, CLASS, DO, ELSE, FALSE, FUNC, FOR, IF, NIL, OR,
		PRINT, RETURN, SUPER, THIS, TRUE, UNDEFINED, VAR, WHILE,

		END_OF_FILE
	};

	//These characters always result in tokens on their own. Chars like '=' could be a "==", so
	//will be handled inside of the scanner
	extern const std::unordered_map<char, TokenType> c_oneCharTokens;

	extern const std::unordered_map<std::string, TokenType> keywords;

	class Token {
	public:
		const TokenType type;
		const std::string lexeme;
		const int line;

		const Value& getLiteralValue();

		std::string toString();

		template<class T>
		Token(TokenType type, const std::string& lexeme, int lineNum, const T& literalValue) :
			type(type),
			lexeme(lexeme),
			line(lineNum)
		{
			value = literalValue;
		}

	private:
		Value value;
	};

	typedef std::vector<Token> TokenList;

	class Scanner {
	public:

		TokenList scanTokens();

		Scanner(const std::string& source, Runtime* runtime);
		Scanner(std::string&& source, Runtime* runtime);

	private:
		Runtime*const runtime;

		const std::string source;
		TokenList tokens;

		int start = 0;
		int current = 0;
		int line = 1;

		bool isAtEnd()const;

		bool isDigit(char c);

		bool isHexDigit(char c);

		bool isAlphabeticalOrUnderscore(char c);

		bool isAlphaNumeric(char c);

		char advance();

		char peek();

		char peekNext();

		void addToken(TokenType type);

		template<class T>
		void addToken(TokenType type, const T& value) {
			tokens.emplace_back(type, getSubstring(source, start, current), line, value);
		}

		bool match(char expected);

		void scanStringLiteral();

		void scanNumericLiteral(char c);

		void scanIdentifier();

		void scanToken();
	};
}