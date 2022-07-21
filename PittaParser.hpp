#pragma once
#include <initializer_list>
#include "PittaStatements.hpp"

namespace pitta {


	class Parser {
	public:

		template<class T, class R>
		std::vector<Stmt<T, R>*> parse() {
			std::vector<Stmt<T, R>*> statements;
			while (!isAtEnd())
				statements.emplace_back(declaration <T, R>());

			return statements;
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
			return peek().type == END_OF_FILE;
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
			return assignment<T>();
		}

		template<class T>
		Expr<T>* assignment() {
			Expr<T>* expr = equality<T>();

			if (match(EQUAL)) {
				Token equals = previous();
				Expr<T>* value = assignment<T>();

				if (expr->getType() == typeid(Variable<T>)) {
					Token name = ((Variable<T>*)expr)->name;
					return new Assign<T>(name, value);
				}

				error(equals, "Invalid assignment target.");
			}

			return expr;
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
			if (match(IDENTIFIER)) return new Variable<T>(previous());

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


		template<class T, class R>
		Stmt<T, R>* declaration() {
			try {
				if (match(VAR))
					return varDeclaration<T, R>();

				return statement<T, R>();
			}
			catch (PittaRuntimeException* error) {
				synchronize();
				return nullptr;
			}
		}

		template<class T,class R>
		Stmt<T, R>* statement() {
			if (match(IF)) return ifStatement<T, R>();
			if (match(LEFT_BRACE))return new Block<T, R>(block<T, R>());
			if (match(PRINT))return printStatement<T, R>();

			return expressionStatement<T, R>();
		}

		template<class T, class R>
		std::vector<Stmt<T, R>*> block() {
			std::vector<Stmt<T, R>*> statements;

			while (!check(RIGHT_BRACE) && !isAtEnd())
				statements.emplace_back(declaration<T, R>());

			consume(RIGHT_BRACE, "Expect '}' after block.");
			return statements;
		}

		template<class T, class R>
		Stmt<T, R>* expressionStatement() {
			Expr<R>* expr = expression<R>();
			consume(SEMICOLON, "Expect ';' after expression");
			return new Expression<T, R>(expr);
		}

		template<class T, class R>
		Stmt<T, R>* ifStatement() {
			consume(LEFT_PAREN, "Expect '(' after 'if'.");
			Expr<R>* condition = expression<R>();
			consume(RIGHT_PAREN, "Expect ')' after if condition.");

			Stmt<T, R>* thenBranch = statement<T, R>();
			Stmt<T, R>* elseBranch = nullptr;
			if (match(ELSE)) {
				elseBranch = statement<T, R>();
			}

			return new If<T, R>(condition, thenBranch, elseBranch);
		}

		template<class T, class R>
		Stmt<T, R>* printStatement() {
			Expr<R>* expr = expression<R>();
			consume(SEMICOLON, "Expect ';' after value");
			return new Print<T, R>(expr);
		}

		template<class T, class R>
		Stmt<T, R>* varDeclaration() {
			Token name = consume(IDENTIFIER, "Expect variable name.");

			Expr<R>* initializer = nullptr;
			if (match(EQUAL)) {
				initializer = expression<R>();
			}

			consume(SEMICOLON, "Expect ';' after variable declaration");
			return new Var<T, R>(name, initializer);
		}

		/*template<class T, class R>
		Stmt<T, R>* statement() {

		}*/
		

	};

}