#pragma once

#include "PittaStatements.hpp"
#include "PittaEnvironment.hpp"
#include "PittaInterpreter.hpp"
#include <stack>

namespace pitta {

	class Resolver : private StatementVisitor<void, Value>, private ExpressionVisitor<Value> {
	public:

		void sweepStatements(const std::vector<Stmt<void, Value>*>& statements) {
			for (Stmt<void, Value>* stmt : statements)
				stmt->accept(this);
		}

		Resolver(Interpreter* interpreter):
			interpreter(interpreter)
		{}

	private:
		Interpreter* interpreter;
		std::vector<std::unordered_map<std::string, bool>> scopes;


		void resolve(std::vector<Stmt<void, Value>*>& statements) {
			for (auto statement : statements) {
				resolve(statement);
			}
		}

		void resolve(Stmt<void, Value>* statement) {
			statement->accept(this);
		}

		void resolve(Expr<Value>* expression) {
			expression->accept(this);
		}

		void resolveLocal(Expr<Value>* expression, const Token& name) {
			for (int i = scopes.size() - 1; i >= 0; i--) {
				if (scopes[i].count(name.lexeme) > 0) {
					interpreter->resolve(expression, scopes.size() - 1 - i);
				}
			}
		}

		void resolveFunction(FunctionStmt<void, Value>* function) {
			beginScope();
			for (const Token& param : function->params) {
				declare(param);
				define(param);
			}
			resolve(function->body);
			endScope();
		}

		void beginScope() {
			scopes.emplace_back();
		}

		void endScope() {
			scopes.pop_back();
		}

		void declare(const Token& name) {
			if (!scopes.empty()) {
				auto& currentScope = scopes.back();
				currentScope.emplace(name.lexeme, false);
			}
		}

		void define(const Token& name) {
			if (!scopes.empty()) {
				auto& currentScope = scopes.back();
				currentScope.emplace(name.lexeme, true);
			}
		}

		Value visitAssignExpr(Assign<Value>* expr) {
			resolve(expr->value);
			resolveLocal(expr, expr->name);
		}

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitCallExpr(Call<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitLogicalExpr(Logical<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		Value visitVariableExpr(Variable<Value>* expr) {
			if (!scopes.empty()) {
				auto& currentScope = scopes.back();
				if (currentScope.at(expr->name.lexeme) == false) {
					interpreter->getRuntime()->runtimeError(new PittaRuntimeException(
						"Can't read local variable in its own initiliser."
					));
				}
			}

			resolveLocal(expr, expr->name);
			return 0;
		}



		void visitBlockStmt(Block<void, Value>* stmt)override {
			beginScope();
			resolve(stmt->statements);
			endScope();
		}

		void visitExpressionStmt(Expression<void, Value>* stmt);

		void visitFunctionStmt(FunctionStmt<void, Value>* stmt) {
			declare(stmt->name);
			define(stmt->name);

			resolveFunction(stmt);
		}

		void visitIfStmt(If<void, Value>* stmt);

		void visitPrintStmt(Print<void, Value>* stmt);

		void visitReturnStmt(Return<void, Value>* stmt);

		void visitVarStmt(Var<void, Value>* stmt) override {
			declare(stmt->name);
			if (stmt->initializer != nullptr) {
				resolve(stmt->initializer);
			}
			define(stmt->name);
		}

		void visitWhileStmt(While<void, Value>* stmt)override;
	};

}