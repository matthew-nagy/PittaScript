#pragma once
#include <initializer_list>
#include "PittaExpressions.hpp"

namespace pitta {


	class Parser {
	public:

		template<class T>
		Expr<T>* parse() {
			try {
				return expression<T>();
			}
			catch (PittaRuntimeException* exception) {
				return nullptr;
			}
		}

		Parser(const std::vector<Token>& tokens, Runtime* runtime) :
			tokens(tokens),
			runtime(runtime)
		{}
		Parser(std::vector<Token>&& tokens, Runtime* runtime) :
			tokens(std::move(tokens)),
			runtime(runtime)
		{}
	private:
		std::vector<Token> tokens;
		int current = 0;

		Runtime* runtime;

		bool isAtEnd() {
			return peek().type == EOF;
		}

		Token peek() {
			return tokens[current];
		}

		Token previous() {
			return tokens[current - 1];
		}

		Token advance() {
			if (!isAtEnd()) current++;
			return previous();
		}

		bool check(TokenType type) {
			if (isAtEnd()) return false;
			return peek().type == type;
		}

		bool match(TokenType tt) {
			if (check(tt)) {
				advance();
				return true;
			}
			return false;
		}

		bool match(const std::initializer_list<TokenType>& tokens) {
			for (TokenType tt : tokens) {
				if (check(tt)) {
					advance();
					return true;
				}
			}
			return false;
		}

		PittaRuntimeException* error(Token token, const std::string& message);

		Token consume(TokenType type, const std::string& message) {
			if (check(type)) return advance();

			throw error(peek(), message);
		}

		void synchronize() {
			advance();

			while (!isAtEnd()) {
				if (previous().type == SEMICOLON) return;

				switch (peek().type) {
				case CLASS:
				case DO:
				case FUNC:
				case VAR:
				case FOR:
				case IF:
				case WHILE:
				case PRINT:
				case RETURN:
					return;
				}

				advance();
			}
		}



		template<class T>
		Expr<T>* expression() {
			return equality<T>();
		}

		template<class T>
		Expr<T>* equality() {
			Expr<T>* expr = comparison<T>();

			while (match({ BANG_EQUAL, EQUAL_EQUAL })) {
				Token op = previous();
				Expr<T>* right = comparison<T>();
				expr = new Binary<T>(expr, op, right);
			}

			return expr;
		}

		template<class T>
		Expr<T>* comparison() {
			Expr<T>* expr = term<T>();

			while (match({ GREATER, GREATER_EQUAL, LESS, LESS_EQUAL })) {
				Token op = previous();
				Expr<T>* right = term<T>();
				expr = new Binary<T>(expr, op, right);
			}

			return expr;
		}

		template<class T>
		Expr<T>* term() {
			Expr<T>* expr = factor<T>();

			while (match({ MINUS, PLUS, PERCENT, BIT_AND, BIT_OR, STRING_CONCAT, SHIFT_LEFT, SHIFT_RIGHT })) {
				Token op = previous();
				Expr<T>* right = factor<T>();
				expr = new Binary<T>(expr, op, right);
			}

			return expr;
		}

		template<class T>
		Expr<T>* factor() {
			Expr<T>* expr = unary<T>();

			while (match({ SLASH, STAR })) {
				Token op = previous();
				Expr<T>* right = unary<T>();
				expr = new Binary<T>(expr, op, right);
			}

			return expr;
		}

		template<class T>
		Expr<T>* unary() {
			if (match({ BANG, MINUS })) {
				Token op = previous();
				Expr<T>* right = unary<T>();
				return new Unary<T>(op, right);
			}

			return primary<T>();
		}

		template<class T>
		Expr<T>* primary() {
			Value value;
			if (match(FALSE)) return new Literal<T>(value = false);
			if (match(TRUE)) return new Literal<T>(value = true);
			if (match(NIL)) return new Literal<T>(value = nullptr);
			if (match(UNDEFINED)) return new Literal<T>(value = Undefined);

			if (match({ INT, FLOAT, STRING })) {
				return new Literal<T>(value = previous().getLiteralValue());
			}

			if (match(LEFT_PAREN)) {
				Expr<T>* expr = expression<T>();
				consume(RIGHT_PAREN, "Expect ')' after expression.");
				return new Grouping<T>(expr);
			}

			throw error(peek(), "Expect expression.");
		}
	};

}