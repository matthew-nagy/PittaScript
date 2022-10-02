#include "PittaTokenScanner.hpp"
#include "PittaRuntime.hpp"

namespace pitta {

	//These characters always result in tokens on their own. Chars like '=' could be a "==", so
	//will be handled inside of the scanner
	const std::unordered_map<char, TokenType> c_oneCharTokens = {
		{ '(', LEFT_PAREN },
		{ ')', RIGHT_PAREN },
		{ '{', LEFT_BRACE },
		{ '}', RIGHT_BRACE },
		{ ',', COMMA },
		{ '.', DOT },
		{ '-', MINUS },
		{ '%', PERCENT },
		{ '|', BIT_OR },
		{ '&', BIT_AND },
		{ '^', BIT_XOR },
		{ '~', BIT_NOT },
		{ ';', SEMICOLON },
		{ '*', STAR },
		{ '/', SLASH}
	};

	const std::string c_classSelfReferenceKey = "this";
	const std::unordered_map<std::string, TokenType> keywords = {
		{ "and", AND },
		{ "class", CLASS },
		{ "do", DO },
		{ "else", ELSE },
		{ "false", FALSE },
		{ "func", FUNC },
		{ "for", FOR },
		{ "if", IF },
		{ "null", NIL },
		{ "or", OR },
		{ "print", PRINT },
		{ "return", RETURN },
		{ "super", SUPER },
		{ c_classSelfReferenceKey, THIS },
		{ "true", TRUE },
		{ "undefined", UNDEFINED },
		{ "var", VAR },
		{ "while", WHILE },
	};

	/*
		LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, SEMICOLON, MINUS, SLASH, STAR, PERCENT,
		BIT_OR, BIT_AND, BIT_XOR, BIT_NOT, BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL,
		LESS, LESS_EQUAL, PLUS, STRING_CONCAT, IDENTIFIER, STRING, INT, FLOAT, AND, CLASS, DO, ELSE, FALSE, 
		FUNC, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS, TRUE, UNDEFINED, VAR, WHILE, END_OF_FILE
	*/

#define TOKEN_TO_STRING(token) { token, #token }

	const std::unordered_map<TokenType, std::string> tokenNames = {
		TOKEN_TO_STRING(LEFT_PAREN), TOKEN_TO_STRING(RIGHT_PAREN), TOKEN_TO_STRING(LEFT_BRACE), TOKEN_TO_STRING(RIGHT_BRACE),
		TOKEN_TO_STRING(COMMA), TOKEN_TO_STRING(DOT), TOKEN_TO_STRING(SEMICOLON), TOKEN_TO_STRING(MINUS), TOKEN_TO_STRING(SLASH),
		TOKEN_TO_STRING(STAR), TOKEN_TO_STRING(PERCENT), TOKEN_TO_STRING(BIT_OR), TOKEN_TO_STRING(BIT_AND), TOKEN_TO_STRING(BIT_XOR),
		TOKEN_TO_STRING(BIT_NOT), TOKEN_TO_STRING(BANG), TOKEN_TO_STRING(BANG_EQUAL), TOKEN_TO_STRING(EQUAL), TOKEN_TO_STRING(EQUAL_EQUAL),
		TOKEN_TO_STRING(GREATER), TOKEN_TO_STRING(GREATER_EQUAL), TOKEN_TO_STRING(SHIFT_RIGHT), 
		TOKEN_TO_STRING(LESS), TOKEN_TO_STRING(LESS_EQUAL), TOKEN_TO_STRING(SHIFT_LEFT), TOKEN_TO_STRING(PLUS),
		TOKEN_TO_STRING(STRING_CONCAT), TOKEN_TO_STRING(IDENTIFIER), TOKEN_TO_STRING(STRING), TOKEN_TO_STRING(INT), TOKEN_TO_STRING(FLOAT),
		TOKEN_TO_STRING(AND), TOKEN_TO_STRING(CLASS), TOKEN_TO_STRING(DO), TOKEN_TO_STRING(ELSE), TOKEN_TO_STRING(FALSE),
		TOKEN_TO_STRING(FUNC), TOKEN_TO_STRING(FOR), TOKEN_TO_STRING(IF), TOKEN_TO_STRING(NIL), TOKEN_TO_STRING(OR), TOKEN_TO_STRING(PRINT),
		TOKEN_TO_STRING(RETURN), TOKEN_TO_STRING(SUPER), TOKEN_TO_STRING(THIS), TOKEN_TO_STRING(TRUE), TOKEN_TO_STRING(UNDEFINED),
		TOKEN_TO_STRING(VAR), TOKEN_TO_STRING(WHILE), TOKEN_TO_STRING(END_OF_FILE)
	};

#undef TOKEN_TO_STRING

	const Value& Token::getLiteralValue() {
		return value;
	}

	std::string Token::toString() {
		return std::to_string(line) +": "+
			tokenNames.at(type) + 
			" '" + 
			lexeme + 
			"' [" +
			value.toString() + 
			"]";
	}




		TokenList Scanner::scanTokens() {
			while (!isAtEnd()) {
				start = current;
				scanToken();
			}

			tokens.emplace_back(END_OF_FILE, "", line, NIL);

			return tokens;
		}

		Scanner::Scanner(const std::string& source, Runtime* runtime) :
			source(source),
			runtime(runtime)
		{}
		Scanner::Scanner(std::string&& source, Runtime* runtime) :
			source(std::move(source)),
			runtime(runtime)
		{}



		bool Scanner::isAtEnd()const {
			return current >= source.size();
		}

		bool Scanner::isDigit(char c) {
			return c >= '0' && c <= '9';
		}

		bool Scanner::isHexDigit(char c) {
			return isDigit(c) || (c >= 'A' && c <= 'F');
		}

		bool Scanner::isAlphabeticalOrUnderscore(char c) {
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'z') || (c == '_');
		}

		bool Scanner::isAlphaNumeric(char c) {
			return isAlphabeticalOrUnderscore(c) || isDigit(c);
		}


		char Scanner::advance() {
			char nextToken = source[current];
			current += 1;
			return nextToken;
		}

		char Scanner::peek() {
			if (isAtEnd())
				return '\0';
			return source[current];
		}

		char Scanner::peekNext() {
			if (current + 1 >= source.length()) return '\0';
			return source[current + 1];
		}

		void Scanner::addToken(TokenType type) {
			addToken(type, Undefined);
		}


		bool Scanner::match(char expected) {
			if (isAtEnd()) return false;
			if (source[current] != expected) return false;

			current++;
			return true;
		}

		void Scanner::scanStringLiteral() {
			while (peek() != '"' && !isAtEnd()) {
				if (peek() == '\n') line++;
				advance();
			}

			if (isAtEnd()) {
				runtime->error(line, "Unterminated string.");
				return;
			}

			// The closing ".
			advance();

			// Trim the surrounding quotes.
			std::string value = getSubstring(source, start + 1, current - 1);
			addToken(STRING, value);
		}

		void Scanner::scanNumericLiteral(char c) {

			bool isHex = false, isBin = false;
			if (c == '0') {
				if (peek() == 'b') {
					isBin = true;
					advance();
				}
				else if (peek() == 'x') {
					isHex = true;
					advance();
				}
			}

			if (isBin)
				if (!(peek() == '0' || peek() == '1'))
					runtime->error(line, "0b must be followed by a valid binary digit");

			if (isHex)
				if (!isHexDigit(peek()))
					runtime->error(line, "0x must be followed by a valid hexidecimal digit");

			if (isHex)
				while (isHexDigit(peek())) advance();
			else
				while (isDigit(peek())) advance();

			bool isFloat = false;

			// Look for a fractional part.
			if (peek() == '.' && isDigit(peekNext())) {

				if (isBin || isHex) {
					runtime->error(line, "Numeric bases cannot be given as a floating point");
				}
				else {
					isFloat = true;
					// Consume the "."
					advance();

					while (isDigit(peek())) advance();
				}
			}

			std::string stringRepresentation = getSubstring(source, start, current);

			if (isFloat)
				addToken<float>(FLOAT, std::atof(stringRepresentation.c_str()));
			else {
				if (isHex || isBin)
					addToken<int>(INT, std::strtol(stringRepresentation.substr(2).c_str(), NULL, isHex ? 16 : 2));
				else
					addToken<int>(INT, std::atoi(stringRepresentation.c_str()));
			}
		}

		void Scanner::scanIdentifier() {
			while (isAlphaNumeric(peek())) advance();

			std::string text = getSubstring(source, start, current);
			TokenType type = IDENTIFIER;
			if (keywords.count(text) > 0)
				type = keywords.at(text);
			addToken(type);
		}

		void Scanner::scanToken() {
			char c = advance();

			if (c_oneCharTokens.count(c) > 0)
				addToken(c_oneCharTokens.at(c));
			else {
				switch (c) {
				case '!':
					addToken(match('=') ? BANG_EQUAL : BANG);
					break;
				case '=':
					addToken(match('=') ? EQUAL_EQUAL : EQUAL);
					break;
				case '+':
					addToken(match('+') ? STRING_CONCAT : PLUS);
					break;
				case '<':
					addToken(match('=') ? LESS_EQUAL : ( match('<') ? SHIFT_LEFT : LESS));
					break;
				case '>':
					addToken(match('=') ? GREATER_EQUAL : (match('>') ? SHIFT_RIGHT : GREATER));
					break;
				case '#':
					while (peek() != '\n' && !isAtEnd())
						advance();

				case ' ':
				case '\r':
				case '\t':
					break;	//Whitespace, ignore it
				case '\n':
					line += 1;
					break;

				case '"':
					scanStringLiteral();
					break;

				default:
					if (isDigit(c))
						scanNumericLiteral(c);
					else if (isAlphabeticalOrUnderscore(c))
						scanIdentifier();
					else
						runtime->error(line, "Unexpected character.");
				}
			}
		}

}