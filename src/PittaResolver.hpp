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
		enum class FunctionType : char {
			NONE,
			FUNCTION,
			INITIALISER,
			METHOD
		};

		enum class ClassType : char {
			NONE,
			CLASS
		};

		Interpreter* interpreter;
		std::vector<std::unordered_map<std::string, bool>> scopes;
		FunctionType currentFunction = FunctionType::NONE;
		ClassType currentClass = ClassType::NONE;


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

		Value visitAssignExpr(Assign<Value>* expr) override {
			resolve(expr->value);
			resolveLocal(expr, expr->name);
			return Null;
		}

		Value visitBinaryExpr(Binary<Value>* expr) override {
			resolve(expr->left);
			resolve(expr->right);
			return Null;
		}

		Value visitCallExpr(Call<Value>* expr) override {
			resolve(expr->callee);
			for (auto& arg : expr->arguments)
				resolve(arg);
			return Null;
		}

		Value visitGetExpr(Get<Value>* expr) override {
			resolve(expr->object);
			return Null;
		}

		Value visitGroupingExpr(Grouping<Value>* expr) override {
			resolve(expr->expression);
			return Null;
		}

		Value visitLiteralExpr(Literal<Value>* expr)override {
			return Null;
		}

		Value visitLogicalExpr(Logical<Value>* expr) override {
			resolve(expr->left);
			resolve(expr->right);
			return Null;
		}

		Value visitSetExpr(Set<Value>* expr) override {
			resolve(expr->value);
			resolve(expr->object);
			return Null;
		}

		Value visitSuperExpr(Super<Value>* expr) override {
			resolveLocal(expr, expr->keyword);
			return Null;
		}

		Value visitThisExpr(This<Value>* expr) override {
#ifdef _DEBUG
			if (currentClass == ClassType::NONE)
				interpreter->getRuntime()->error(expr->keyword, "'" + c_classSelfReferenceKey + "' keyword is ony allowed inside classes.");
#endif
			resolveLocal(expr, expr->keyword);
			return Null;
		}

		Value visitUnaryExpr(Unary<Value>* expr) override {
			resolve(expr->right);
			return Null;
		}

		Value visitVariableExpr(Variable<Value>* expr) override {
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
			return Null;
		}



		void visitBlockStmt(Block<void, Value>* stmt)override {
			beginScope();
			resolve(stmt->statements);
			endScope();
		}

		void visitClassStmt(ClassStmt<void, Value>* stmt)override {
			ClassType enclosingClassType = currentClass;
			currentClass = ClassType::CLASS;

			declare(stmt->name);
			define(stmt->name);

			if (stmt->superclass != nullptr) {
				if (stmt->superclass->name.lexeme == stmt->name.lexeme) {
					interpreter->getRuntime()->error(stmt->name, "A class cannot inherit from itself");
				}

				resolve(stmt->superclass);

				beginScope();
				scopes.back().emplace("super", true);
			}


			beginScope();
			scopes.back().emplace(c_classSelfReferenceKey, true);

			for (FunctionStmt<void, Value>* method : stmt->methods) {
				FunctionType declaration = method->name.lexeme == "init" ? FunctionType::INITIALISER : FunctionType::METHOD;
				resolveFunction(method, declaration);
			}

			endScope();
			if (stmt->superclass != nullptr)
				endScope();

			currentClass = enclosingClassType;
		}

		void visitExpressionStmt(Expression<void, Value>* stmt) override {
			resolve(stmt->expression);
		}

		void visitFunctionStmt(FunctionStmt<void, Value>* stmt) override {
			declare(stmt->name);
			define(stmt->name);

			resolveFunction(stmt, FunctionType::FUNCTION);
		}

		void visitIfStmt(If<void, Value>* stmt) override {
			resolve(stmt->condition);
			resolve(stmt->thenBranch);
			if (stmt->elseBranch != nullptr)
				resolve(stmt->elseBranch);
		}

		void visitPrintStmt(Print<void, Value>* stmt) override {
			resolve(stmt->expression);
		}

		void visitReturnStmt(Return<void, Value>* stmt) override {
#ifdef _DEBUG
			if (currentFunction == FunctionType::NONE) {
				interpreter->getRuntime()->error(stmt->keyword, "Can't return from top level code");
			}
			if (currentFunction == FunctionType::INITIALISER && stmt->value != nullptr) {
				interpreter->getRuntime()->error(stmt->keyword, "Can't return a value from an object initialiser");
			}
#endif
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