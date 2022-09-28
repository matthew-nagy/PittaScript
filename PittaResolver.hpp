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
		enum class FunctionType {
			None,
			Function
		};

		Interpreter* interpreter;
		std::vector<std::unordered_map<std::string, bool>> scopes;
		FunctionType currentFunction = FunctionType::None;


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

		void resolveFunction(FunctionStmt<void, Value>* function, FunctionType type) {
			FunctionType enclosingFunctionType = currentFunction;
			currentFunction = type;

			beginScope();
			for (const Token& param : function->params) {
				declare(param);
				define(param);
			}
			resolve(function->body);
			endScope();

			currentFunction = enclosingFunctionType;
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
				if (currentScope.count(name.lexeme) > 0) {
					interpreter->getRuntime()->error(name, 
						"Variable '" + name.lexeme + "' already declared in this scope"
					);
				}

				currentScope.emplace(name.lexeme, false);
			}
		}

		void define(const Token& name) {
			if (!scopes.empty()) {
				auto& currentScope = scopes.back();

				currentScope.at(name.lexeme) = true;
			}
		}

		Value visitAssignExpr(Assign<Value>* expr) {
			resolve(expr->value);
			resolveLocal(expr, expr->name);
			return 0;
		}

		Value visitBinaryExpr(Binary<Value>* expr) {
			resolve(expr->left);
			resolve(expr->right);
			return 0;
		}

		Value visitCallExpr(Call<Value>* expr) {
			resolve(expr->callee);
			for (auto& arg : expr->arguments)
				resolve(arg);
			return 0;
		}

		Value visitGroupingExpr(Grouping<Value>* expr) {
			resolve(expr->expression);
			return 0;
		}

		Value visitLiteralExpr(Literal<Value>* expr){
			return 0;
		}

		Value visitLogicalExpr(Logical<Value>* expr) {
			resolve(expr->left);
			resolve(expr->right);
			return 0;
		}

		Value visitUnaryExpr(Unary<Value>* expr) {
			resolve(expr->right);
			return 0;
		}

		Value visitVariableExpr(Variable<Value>* expr) {
			if (!scopes.empty()) {
				auto& currentScope = scopes.back();
				if (currentScope.count(expr->name.lexeme) > 0) {
					if (currentScope.at(expr->name.lexeme) == false) {
						interpreter->getRuntime()->error(expr->name,
							"Can't read local variable in its own initiliser. Variable '" + expr->name.lexeme + "' On line " + std::to_string(expr->name.line)
						);
					}
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

		void visitClassStmt(ClassStmt<void, Value>* stmt)override {
			declare(stmt->name);
			define(stmt->name);
		}

		void visitExpressionStmt(Expression<void, Value>* stmt) {
			resolve(stmt->expression);
		}

		void visitFunctionStmt(FunctionStmt<void, Value>* stmt) {
			declare(stmt->name);
			define(stmt->name);

			resolveFunction(stmt, FunctionType::Function);
		}

		void visitIfStmt(If<void, Value>* stmt) {
			resolve(stmt->condition);
			resolve(stmt->thenBranch);
			if (stmt->elseBranch != nullptr)
				resolve(stmt->elseBranch);
		}

		void visitPrintStmt(Print<void, Value>* stmt) {
			resolve(stmt->expression);
		}

		void visitReturnStmt(Return<void, Value>* stmt) {
			if (currentFunction == FunctionType::None) {
				interpreter->getRuntime()->error(stmt->keyword, "Can't return from top level code");
			}
			if (stmt->value != nullptr)
				resolve(stmt->value);
		}

		void visitVarStmt(Var<void, Value>* stmt) override {
			declare(stmt->name);
			if (stmt->initializer != nullptr) {
				resolve(stmt->initializer);
			}
			define(stmt->name);
		}

		void visitWhileStmt(While<void, Value>* stmt)override {
			resolve(stmt->condition);
			resolve(stmt->body);
		}
	};

}