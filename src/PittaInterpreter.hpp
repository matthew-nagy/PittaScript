#pragma once
#include "PittaStatements.hpp"
#include "PittaEnvironment.hpp"
#include "PittaRuntime.hpp"

namespace pitta {
	class Resolver;
	class Class;
	class Instance;

	class Interpreter final : public ExpressionVisitor<Value>, public StatementVisitor<void, Value> {
		friend class Resolver;
	public:

		Value visitAssignExpr(Assign<Value>* expr);

		Value visitBinaryExpr(Binary<Value>* expr);

		Value visitCallExpr(Call<Value>* expr);

		Value visitGetExpr(Get<Value>* expr);

		Value visitGroupingExpr(Grouping<Value>* expr);

		Value visitLiteralExpr(Literal<Value>* expr);

		Value visitLogicalExpr(Logical<Value>* expr);

		Value visitSetExpr(Set<Value>* expr);

		Value visitSuperExpr(Super<Value>* expr);

		Value visitThisExpr(This<Value>* expr);

		Value visitUnaryExpr(Unary<Value>* expr);

		Value visitVariableExpr(Variable<Value>* expr);


		
		void visitBlockStmt(Block<void, Value>* stmt);

		void visitClassStmt(ClassStmt<void, Value>* stmt);

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

		void registerNewInstance(Instance* newInstance);

		Interpreter(Runtime* runtime);
		Interpreter(Runtime* runtime, const std::shared_ptr<Environment>& globals);
		~Interpreter();
	private:
		Runtime* runtime;

		//While running, thiiings could be made. We want to collect them all so we can delete them in the end
		std::vector<Callable*> generatedCallables;
		std::vector<Class*> generatedClasses;
		std::vector<Instance*> generatedInstances;

		std::shared_ptr<Environment> globals;
		std::shared_ptr<Environment> environment;
		std::unordered_map<Expr<Value>*, int> locals;

		Value evaluate(Expr<Value>* expression);

		void execute(Stmt<void, Value>* stmt);

		void resolve(Expr<Value>* expression, int depth);

		Value lookUpVariable(const Token& name, Expr<Value>* expr);
	};
}