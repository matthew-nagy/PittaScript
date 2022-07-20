#pragma once
#include "PittaExpressions.hpp"

namespace pitta {
	class Interpreter : public ExpressionVisitor<Value> {
	public:

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		void interpret(Expr<Value>* expression);

		Interpreter(Runtime* runtime);
	private:
		Runtime* runtime;

		Value evaluate(Expr<Value>* expression);
	};
}