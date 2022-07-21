#pragma once
#include "PittaStatements.hpp"
#include "PittaEnvironment.hpp"

namespace pitta {
	class Interpreter : public ExpressionVisitor<Value>, public StatementVisitor<void, Value> {
	public:

		Value visitAssignExpr(Assign<Value>* expr);

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		Value visitVariableExpr(Variable<Value>* expr);


		
		void visitBlockStmt(Block<void, Value>* stmt);

		void visitExpressionStmt(Expression<void, Value>* stmt);

		void visitPrintStmt(Print<void, Value>* stmt);

		void visitVarStmt(Var<void, Value>* stmt);

		void interpret(Expr<Value>* expression);
		void interpret(const std::vector<Stmt<void, Value>*>& statements);

		Interpreter(Runtime* runtime);
	private:
		Runtime* runtime;

		std::shared_ptr<Environment> environment = std::make_shared<Environment>();

		Value evaluate(Expr<Value>* expression);

		void execute(Stmt<void, Value>* stmt);

		void executeBlock(const std::vector<Stmt<void, Value>*>& statements, const std::shared_ptr<Environment>& newEnv);
	};
}