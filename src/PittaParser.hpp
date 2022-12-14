#pragma once
#include <initializer_list>
#include "PittaStatements.hpp"
#include "PittaRuntime.hpp"

#ifndef PITTA_MAX_FUNC_PARAMS
#define PITTA_MAX_FUNC_PARAMS 255
#endif

namespace pitta {

	template<class T, class R>
	class AbstractSyntaxTree {
	public:
		const std::vector<Stmt<T, R>*> statements;

		AbstractSyntaxTree(const std::vector<Stmt<T, R>*>& statements,
				const std::vector<Stmt<T, R>*>& allStatementPointers, 
				const std::vector<Expr<R>*>& allExpressionPointers):
			statements(statements),
			allStatementPointers(allStatementPointers),
			allExpressionPointers(allExpressionPointers)
		{}
		
		~AbstractSyntaxTree() {
			for (Stmt<T, R>* stmt : allStatementPointers)
				delete stmt;
			for (Expr<R>* expr : allExpressionPointers)
				delete expr;
		}

	private:
		std::vector<Stmt<T, R>*> allStatementPointers;
		std::vector<Expr<R>*> allExpressionPointers;
	};

	template<class T, class R>
	class Parser {
	public:

		AbstractSyntaxTree<T, R> parse() {
			allExpressions.clear();
			allStatements.clear();
			std::vector<Stmt<T, R>*> statements;
			while (!isAtEnd())
				statements.emplace_back(declaration());

			return AbstractSyntaxTree<T, R>(statements, allStatements, allExpressions);
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
		std::vector<Expr<R>*> allExpressions;
		std::vector<Stmt<T, R>*> allStatements;

		Expr<R>* track(Expr<R>* expr) {
			allExpressions.emplace_back(expr);
			return expr;
		}
		Stmt<T, R>* track(Stmt<T, R>* stmt) {
			allStatements.emplace_back(stmt);
			return stmt;
		}



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

		PittaRuntimeException* error(Token token, const std::string& message) {
			runtime->error(token, message);
			return new PittaRuntimeException("See log for detail");
		}

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
				default:
					break;
				}

				advance();
			}
		}


		Expr<R>* finishCall(Expr<R>* callee) {
			std::vector<Expr<R>*> arguments;
			if (!check(RIGHT_PAREN)) {
				do {
					arguments.emplace_back(expression());
				} while (match(COMMA));
			}

			Token paren = consume(RIGHT_PAREN,
				"Expect ')' after arguments.");

			return new Call<R>(callee, paren, arguments);
		}


		Expr<R>* expression() {
			return assignment();
		}

		Expr<R>* assignment() {
			Expr<R>* expr = orExpr();

			if (match(EQUAL)) {
				Token equals = previous();
				Expr<R>* value = assignment();

				if (expr->getType() == typeid(Variable<R>)) {
					Token name = ((Variable<R>*)expr)->name;
					return track(new Assign<R>(name, value));
				}
				else if (expr->getType() == typeid(Get<R>)) {
					Get<R>* get = (Get<R>*)expr;
					return track(new Set<R>(get->object, get->name, value));
				}

				error(equals, "Invalid assignment target.");
			}

			return expr;
		}

		Expr<R>* orExpr() {
			Expr<R>* expr = andExpr();

			while (match(OR)) {
				Token op = previous();
				Expr<R>* right = andExpr();
				expr = track(new Logical<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* andExpr(){
			Expr<R>* expr = equality();

			while (match(AND)) {
				Token op = previous();
				Expr<R>* right = equality();
				expr = track(new Logical<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* equality() {
			Expr<R>* expr = comparison();

			while (match({ BANG_EQUAL, EQUAL_EQUAL })) {
				Token op = previous();
				Expr<R>* right = comparison();
				expr = track(new Binary<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* comparison() {
			Expr<R>* expr = term();

			while (match({ GREATER, GREATER_EQUAL, LESS, LESS_EQUAL })) {
				Token op = previous();
				Expr<R>* right = term();
				expr = track(new Binary<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* term() {
			Expr<R>* expr = factor();

			while (match({ MINUS, PLUS, PERCENT, BIT_AND, BIT_OR, STRING_CONCAT, SHIFT_LEFT, SHIFT_RIGHT })) {
				Token op = previous();
				Expr<R>* right = factor();
				expr = track(new Binary<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* factor() {
			Expr<R>* expr = unary();

			while (match({ SLASH, STAR })) {
				Token op = previous();
				Expr<R>* right = unary();
				expr = track(new Binary<R>(expr, op, right));
			}

			return expr;
		}

		Expr<R>* unary() {
			if (match({ BANG, MINUS })) {
				Token op = previous();
				Expr<R>* right = unary();
				return track(new Unary<R>(op, right));
			}

			return call();
		}

		Expr<R>* call() {
			Expr<R>* expr = primary();

			while (true) {
				if (match(LEFT_PAREN)) {
					expr = finishCall(expr);
				}
				else if (match(DOT)) {
					Token name = consume(IDENTIFIER, "Expect property name after '.'.");
					expr = track(new Get<R>(expr, name));
				}
				else
					break;
			}

			return expr;
		}

		Expr<R>* primary() {
			if (match(FALSE)) return track(new Literal<R>(false));
			if (match(TRUE)) return track(new Literal<R>(true));
			if (match(NIL)) return track(new Literal<R>(Null));
			if (match(UNDEFINED)) return track(new Literal<R>(Undefined));
			if (match(THIS))return track(new This<R>(previous()));
			if (match(IDENTIFIER)) return track(new Variable<R>(previous()));

			if (match({ INT, FLOAT, STRING })) {
				return track(new Literal<R>(previous().getLiteralValue()));
			}

			if (match(SUPER)) {
				Token keyword = previous();
				consume(DOT, "Expect '.' after 'super'.");
				Token method = consume(IDENTIFIER,
					"Expect superclass method name.");
				return track(new Super<R>(keyword, method));
			}

			if (match(LEFT_PAREN)) {
				Expr<R>* expr = expression();
				consume(RIGHT_PAREN, "Expect ')' after expression.");
				return track(new Grouping<R>(expr));
			}

			throw error(peek(), "Expect expression.");
		}


		Stmt<T, R>* declaration() {
			try {
				if (match(CLASS))
					return classDeclaration();
				if (match(VAR))
					return varDeclaration();
				if (match(FUNC))
					return function("function");
				return statement();
			}
			catch (PittaRuntimeException* error) {
				printf("Caught runtime error %s\n", error->details.c_str());

				synchronize();
				return nullptr;
			}
		}

		Stmt<T, R>* statement() {
			if (match(IF)) return ifStatement();
			if (match(LEFT_BRACE))return track(new Block<T, R>(block()));
			if (match(WHILE))return whileStatement();
			if (match(FOR))return forStatement();
			if (match(PRINT))return printStatement();
			if (match(RETURN))return returnStatement();

			return expressionStatement();
		}

		std::vector<Stmt<T, R>*> block() {
			std::vector<Stmt<T, R>*> statements;

			while (!check(RIGHT_BRACE) && !isAtEnd())
				statements.emplace_back(declaration());

			consume(RIGHT_BRACE, "Expect '}' after block.");
			return statements;
		}

		Stmt<T, R>* classDeclaration() {
			Token name = consume(IDENTIFIER, "Expected a class name");

			Variable<R>* superclass = nullptr;
			if (match(LESS)) {
				consume(IDENTIFIER, "Expect superclass name");
				superclass = new Variable<R>(previous());
				track(superclass);
			}

			consume(LEFT_BRACE, "Expect '{' before class body");

			std::vector<FunctionStmt<T, R>*> methods;
			while (!check(RIGHT_BRACE) && !isAtEnd()) {
				methods.emplace_back((FunctionStmt<T, R>*)function("method"));
			}

			consume(RIGHT_BRACE, "Expect '}' after class body.");

			return track(new ClassStmt<T, R>(name, superclass, methods));
		}

		Stmt<T, R>* expressionStatement() {
			Expr<R>* expr = expression();
			consume(SEMICOLON, "Expect ';' after expression");
			return track(new Expression<T, R>(expr));
		}

		Stmt<T, R>* forStatement() {
			consume(LEFT_PAREN, "Expect '(' after 'for'.");
			Stmt<T, R>* initializer = nullptr;
			if (match(VAR))
				initializer = varDeclaration();
			else if (!match(SEMICOLON))
				initializer = expressionStatement();

			Expr<R>* condition;
			if (!check(SEMICOLON))
				condition = expression();
			else
				condition = track(new Literal<R>(true));
			consume(SEMICOLON, "Expect ';' after loop condition.");

			Expr<R>* increment;
			if (!check(RIGHT_PAREN))
				increment = expression();
			else
				increment = track(new Literal<R>(true));
			consume(RIGHT_PAREN, "Expect ')' after for clauses.");

			Stmt<T, R>* loopBody = statement();


			loopBody = track(new Block<T, R>({ loopBody, track(new Expression<T, R>(increment)) }));
			loopBody = track(new While<T, R>(condition, loopBody));
			if (initializer != nullptr)
				loopBody = track(new Block<T, R>({ initializer, loopBody }));

			return loopBody;
		}

		Stmt<T, R>* function(const std::string& functionKind) {
			Token name = consume(IDENTIFIER, "Expect " + functionKind + " name.");
			consume(LEFT_PAREN, "Expect '(' after " + functionKind + " name.");
			std::vector<Token> parameters;
			if (!check(RIGHT_PAREN)) {
				do {
					if (parameters.size() >= PITTA_MAX_FUNC_PARAMS) {
						error(peek(), "Can't have more than " + std::to_string(PITTA_MAX_FUNC_PARAMS) + " parameters.");
					}

					parameters.emplace_back(
						consume(IDENTIFIER, "Expect parameter name."));
				} while (match(COMMA));
			}
			consume(RIGHT_PAREN, "Expect ')' after parameters.");
			consume(LEFT_BRACE, "Expect '{' before " + functionKind + " body.");
			std::vector<Stmt<T, R>*> body = block();
			return track(new FunctionStmt<T, R>(name, parameters, body));
		}

		Stmt<T, R>* ifStatement() {
			consume(LEFT_PAREN, "Expect '(' after 'if'.");
			Expr<R>* condition = expression();
			consume(RIGHT_PAREN, "Expect ')' after if condition.");

			Stmt<T, R>* thenBranch = statement();
			Stmt<T, R>* elseBranch = nullptr;
			if (match(ELSE)) {
				elseBranch = statement();
			}

			return track(new If<T, R>(condition, thenBranch, elseBranch));
		}

		Stmt<T, R>* printStatement() {
			Expr<R>* expr = expression();
			consume(SEMICOLON, "Expect ';' after value");
			return track(new Print<T, R>(expr));
		}

		Stmt<T, R>* returnStatement() {
			Token keyword = previous();
			Expr<R>* value = nullptr;
			if (!check(SEMICOLON)) {
				value = expression();
			}

			consume(SEMICOLON, "Expect ';' after return value.");
			return track(new Return<T, R>(keyword, value));
		}

		Stmt<T, R>* varDeclaration() {
			Token name = consume(IDENTIFIER, "Expect variable name.");

			Expr<R>* initializer = nullptr;
			if (match(EQUAL)) {
				initializer = expression();
			}

			consume(SEMICOLON, "Expect ';' after variable declaration");
			return track(new Var<T, R>(name, initializer));
		}

		Stmt<T, R>* whileStatement() {
			consume(LEFT_PAREN, "Expect '(' after 'while'.");
			Expr<R>* condition = expression();
			consume(RIGHT_PAREN, "Expect ')' after condition.");
			Stmt<T, R>* body = statement();

			return track(new While<T, R>(condition, body));
		}

		/*template<class T, class R>
		Stmt<T, R>* statement() {

		}*/
		

	};

}