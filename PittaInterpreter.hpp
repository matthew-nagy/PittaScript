#pragma once
#include "PittaStatements.hpp"
#include "PittaEnvironment.hpp"
#include "PittaRuntime.hpp"

namespace pitta {
	class Interpreter final : public ExpressionVisitor<Value>, public StatementVisitor<void, Value> {
	public:

		Value visitAssignExpr(Assign<Value>* expr);

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitCallExpr(Call<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitLogicalExpr(Logical<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		Value visitVariableExpr(Variable<Value>* expr);


		
		void visitBlockStmt(Block<void, Value>* stmt);

		void visitExpressionStmt(Expression<void, Value>* stmt);

		void visitFunctionStmt(FunctionStmt<void, Value>* stmt);

		void visitIfStmt(If<void, Value>* stmt);

		void visitPrintStmt(Print<void, Value>* stmt);

		void visitReturnStmt(Return<void, Value>* stmt);

		void visitVarStmt(Var<void, Value>* stmt);

		void visitWhileStmt(While<void, Value>* stmt);

		void interpret(Expr<Value>* expression);
		void interpret(const std::vector<Stmt<void, Value>*>& statements);

		void executeBlock(const std::vector<Stmt<void, Value>*>& statements, const std::shared_ptr<Environment>& newEnv);

		Runtime* getRuntime();

		Interpreter(Runtime* runtime);
		Interpreter(Runtime* runtime, const std::shared_ptr<Environment>& globals);
	private:
		Runtime* runtime;

		//While running, callables could be made. We want to collect them all so we can delete them in the end
		std::vector<Callable*> generatedCallables;

		std::shared_ptr<Environment> globals;
		std::shared_ptr<Environment> environment;

		Value evaluate(Expr<Value>* expression);

		void execute(Stmt<void, Value>* stmt);
	};
}