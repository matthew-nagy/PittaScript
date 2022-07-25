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

		Resolver(Runtime* runtime):
			runtime(runtime)
		{}

	private:
		Value visitAssignExpr(Assign<Value>* expr) {
			resolve(expr->value);
			auto relativeLocation = resolveLocal(expr->name);

			expr->environmentDepth = relativeLocation.environmentDepth;
			expr->variableId = relativeLocation.declarationNumber;

			return Undefined;
		}

		Value visitBinaryExpr(Binary<Value>* expr) {
			resolve(expr->left);
			resolve(expr->right);
			return Undefined;
		}

		Value visitCallExpr(Call<Value>* expr) {
			resolve(expr->callee);
			for (Expr<Value>* val : expr->arguments)
				resolve(val);
			return Undefined;
		}

		Value visitGroupingExpr(Grouping<Value>* expr) {
			resolve(expr->expression);
			return Undefined;
		}

		Value visitLiteralExpr(Literal<Value>* expr) {
			return Undefined;
		}

		Value visitLogicalExpr(Logical<Value>* expr) {
			resolve(expr->left);
			resolve(expr->right);
			return Undefined;
		}

		Value visitUnaryExpr(Unary<Value>* expr) {
			resolve(expr->right);
			return Undefined;
		}

		Value visitVariableExpr(Variable<Value>* expr) {
			if (scopes.size() > 0) {
				auto& currentScope = scopes.back();
				if (currentScope.count(expr->name.lexeme) == 0) {
					//runtime->error(expr->name, "Can't read local variable in its own initializer.");
				}
			}

			auto relativeLocation = resolveLocal(expr->name);
			expr->environmentDepth = relativeLocation.environmentDepth;
			expr->variableId = relativeLocation.declarationNumber;

			return Undefined;
		}



		void visitBlockStmt(Block<void, Value>* stmt)override {
			beginScope();
			resolve(stmt->statements);
			endScope();
		}

		void visitExpressionStmt(Expression<void, Value>* stmt) {
			resolve(stmt->expression);
		}

		void visitFunctionStmt(FunctionStmt<void, Value>* stmt) {
			stmt->variableId = declare(stmt->name);
			define(stmt->name);

			resolveFunction(stmt);
		}

		void visitIfStmt(If<void, Value>* stmt) {
			resolve(stmt->condition);
			resolve(stmt->thenBranch);
			if(stmt->elseBranch != nullptr)
				resolve(stmt->elseBranch);
		}

		void visitPrintStmt(Print<void, Value>* stmt) {
			resolve(stmt->expression);
		}

		void visitReturnStmt(Return<void, Value>* stmt) {
			if (stmt->value != nullptr)
				resolve(stmt->value);
		}

		void visitVarStmt(Var<void, Value>* stmt) override {
			stmt->variableId = declare(stmt->name);
			if (stmt->initializer != nullptr) {
				resolve(stmt->initializer);
			}
			define(stmt->name);
		}

		void visitWhileStmt(While<void, Value>* stmt)override {
			resolve(stmt->condition);
			resolve(stmt->body);
		}


		struct Declarement {
			bool defined;
			uint16_t declarationNumber;

			Declarement(bool defined, uint16_t declarationNumber):
				defined(defined),
				declarationNumber(declarationNumber)
			{}
		};
		struct RelativeVariableAddress {
			uint16_t environmentDepth;
			uint16_t declarationNumber;
			RelativeVariableAddress(uint16_t environmentDepth, uint16_t declarationNumber):
				environmentDepth(environmentDepth),
				declarationNumber(declarationNumber)
			{}
		};

		Runtime* runtime;
		std::unordered_map<std::string, Declarement> globalScope;
		std::vector<std::unordered_map<std::string, Declarement>> scopes;

		void resolve(Expr<Value>* expr) {
			expr->accept(this);
		}

		void resolve(Stmt<void, Value>* statement) {
			statement->accept(this);
		}

		void resolve(const std::vector<Stmt<void, Value>*>& statements) {
			for (Stmt<void, Value>* statement : statements) {
				resolve(statement);
			}
		}

		void beginScope() {
			scopes.emplace_back();
		}
		void endScope() {
			scopes.pop_back();
		}

		uint16_t declare(Token& name) {
			if (scopes.empty()) {
				if (!globalScope.count(name.lexeme) > 0) {
					uint16_t id = globalScope.size();
					globalScope.emplace(name.lexeme, Declarement(false, id));
					return id;
				}
			}
			else {
				auto& currentScope = scopes.back();
				uint16_t id = currentScope.size();
				currentScope.emplace(name.lexeme, Declarement(false, id));
				return id;
			}
		}

		void define(Token& name) {
			if (scopes.empty()) {
				globalScope.at(name.lexeme).defined = true;
			}
			else {
				auto& currentScope = scopes.back();
				currentScope.at(name.lexeme).defined = true;
			}
		}

		RelativeVariableAddress resolveLocal(Token& name) {
			if (globalScope.count(name.lexeme) > 0)
				return RelativeVariableAddress(0, globalScope.at(name.lexeme).declarationNumber);
			else
				for (int i = scopes.size() - 1; i >= 0; i--)
					if (scopes[i].count(name.lexeme) > 0)
						return RelativeVariableAddress(i + 1, scopes[i].at(name.lexeme).declarationNumber);
				

			runtime->error(name, "Variable used before its declaration.");
			throw new PittaRuntimeException("Variable used before its declaration");
		}
		void resolveFunction(FunctionStmt<void, Value>* function) {
			beginScope();
			for (Token& param : function->params) {
				function->paramIds.emplace_back(declare(param));
				define(param);
			}
			resolve(function->body);
			endScope();
		}
	};

}