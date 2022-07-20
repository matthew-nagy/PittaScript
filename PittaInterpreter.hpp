#pragma once
#include "PittaStatements.hpp"

namespace pitta {
	class Interpreter : public ExpressionVisitor<Value>, public StatementVisitor<void, Value> {
	public:

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		void visitExpressionStmt(Expression<void, Value>* stmt);

		void visitPrintStmt(Print<void, Value>* stmt);

		void interpret(Expr<Value>* expression);
		void interpret(const std::vector<Stmt<void, Value>*>& statements);

		Interpreter(Runtime* runtime);
	private:
		Runtime* runtime;

		Value evaluate(Expr<Value>* expression);

		void execute(Stmt<void, Value>* stmt);
	};
}