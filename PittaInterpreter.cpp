#include "PittaInterpreter.hpp"
#include "PittaRuntime.hpp"
#include "PittaFunction.hpp"
#include "PittaClass.hpp"

namespace pitta {

	Value Interpreter::visitAssignExpr(Assign<Value>* expr) {
		Value value = evaluate(expr->value);

		auto strval = value.toString();

		if (locals.count(expr) == 0) {
			globals->assign(expr->name, value);
		}
		else {
			int distance = locals.at(expr);
			environment->assignAt(distance, expr->name, value);
		}

		return value;
	}

	Value Interpreter::visitBinaryExpr(Binary<Value>* expr) {
		Value left = evaluate(expr->left);
		Value right = evaluate(expr->right);

		const std::string invalidTypeMsg = "Invalid numeric types for operator";

#define Numeric_Maths_Switch(OpType, Symbol)\
			case OpType:\
				switch (left.getType()) {\
				case Int:\
					return left.asInt() Symbol right.asInt();\
				case Float:\
					return left.asFloat() Symbol right.asFloat();\
				default:\
					printf("Values are %s (%s) and %s (%s)\n", left.toString().c_str(), c_typeToString.at(left.getType()).c_str(), right.toString().c_str(), c_typeToString.at(right.getType()).c_str());\
					runtime->error(expr->op, invalidTypeMsg);\
					throw new PittaRuntimeException(invalidTypeMsg);\
				}\
				break
#define Integer_Maths_Switch(OpType, Symbol)\
case OpType:\
	if(left.getType() != Int || right.getType() != Int){\
		runtime->error(expr->op, "Operator can only interact with integer values");\
		throw new PittaRuntimeException("Operator can only interact with integer values");\
	}\
	else\
		return left.asInt() Symbol right.asInt();\
	break

		switch (expr->op.type) {
			Numeric_Maths_Switch(MINUS, -);
			Numeric_Maths_Switch(PLUS, +);
			Numeric_Maths_Switch(SLASH, / );
			Numeric_Maths_Switch(STAR, *);

		case EQUAL_EQUAL:
			return left == right;
		case BANG_EQUAL:
			return !(left == right);
			Numeric_Maths_Switch(LESS, < );
			Numeric_Maths_Switch(GREATER, > );
			Numeric_Maths_Switch(LESS_EQUAL, <= );
			Numeric_Maths_Switch(GREATER_EQUAL, >= );

		case SHIFT_LEFT:
			printf("%d << %d = %d\n", left.asInt(), right.asInt(), left.asInt() << right.asInt());
			return left.asInt() << right.asInt();
		case SHIFT_RIGHT:
			return left.asInt() >> right.asInt();

		case STRING_CONCAT:
			if (left.getType() != String || right.getType() != String) {
				runtime->error(expr->op, "Non string values cannot undergo string style concatination");
				throw new PittaRuntimeException("Non string values cannot undergo string style concatination");
			}
			else {
				return left.asString() + right.asString();
			}
			Integer_Maths_Switch(BIT_AND, &);
			Integer_Maths_Switch(BIT_OR, | );
			Integer_Maths_Switch(BIT_XOR, ^);
			Integer_Maths_Switch(PERCENT, %);
		default:
			runtime->error(expr->op, "Unknown binary operator requested");
			throw new PittaRuntimeException("Unknown binary operator requested");
		}
#undef Numeric_Maths_Switch
	}

	Value Interpreter::visitCallExpr(Call<Value>* expr) {
		Value callee = evaluate(expr->callee);

		std::vector<Value> arguments;
		arguments.reserve(expr->arguments.size());
		for (Expr<Value>* arg : expr->arguments)
			arguments.emplace_back(evaluate(arg));

		if (callee.getType() != Function && callee.getType() != ClassDef) {
			runtime->error(expr->closingParenthesis, "Can only call functions and classes.");
			throw new PittaRuntimeException("Can only call functions and classes.");
		}

		if (callee.asCallable()->getArity() != arguments.size()) {
			std::string errMsg = "Incorrect number of arguments in function call. Passed in "
				+ std::to_string(arguments.size()) + ", expected "
				+ std::to_string(callee.asCallable()->getArity()) + ".";
			runtime->error(expr->closingParenthesis, errMsg);
			throw new PittaRuntimeException(errMsg);
		}

		return (*callee.asCallable())(this, arguments);
	}

	Value Interpreter::visitGetExpr(Get<Value>* expr) {
		Value object = evaluate(expr->object);
		if (object.getType() == ClassInstance)
			return object.asInstance()->get(expr->name);

		throw new PittaRuntimeException("On '" + expr->name.lexeme + "': Only instances have properties.");
		return Null;
	}

	Value Interpreter::visitGroupingExpr(Grouping<Value>* expr) {
		return evaluate(expr->expression);
	}

	Value Interpreter::visitLogicalExpr(Logical<Value>* expr) {
		Value left = evaluate(expr->left);

		if (expr->op.type == OR) 
			return left.isTruthy() ? left : evaluate(expr->right);
		
		return left.isTruthy() ? evaluate(expr->right) : left;
	}

	Value Interpreter::visitLiteralExpr(Literal<Value>* expr) {
		return expr->value;
	}

	Value Interpreter::visitUnaryExpr(Unary<Value>* expr) {
		Value right = evaluate(expr->right);

		switch (expr->op.type) {
		case MINUS:
			switch (right.getType()) {
			case Int:
				return -1 * right.asInt();
			case Float:
				return -1.0f * right.asFloat();
			default:
				runtime->error(expr->op, "Negation must be followed by and integer or floating point value");
				throw new PittaRuntimeException("Negation must be followed by and integer or floating point value");
			}
			break;
		case BANG:
			return !right.isTruthy();
		case BIT_NOT:
			if (right.getType() == Int) {
				return ~right.asInt();
			}
			else {
				runtime->error(expr->op, "Logical not operator may only be applied to integer values");
				throw new PittaRuntimeException("Logical not operator may only be applied to integer values");
			}
		}

		return Undefined;
	}

	Value Interpreter::visitVariableExpr(Variable<Value>* expr) {
		return lookUpVariable(expr->name, expr);
	}

	Value Interpreter::evaluate(Expr<Value>* expression) {
		return expression->accept(this);
	}








	void Interpreter::visitBlockStmt(Block<void, Value>* stmt) {
		executeBlock(stmt->statements, std::make_shared<Environment>(environment));
	}

	void Interpreter::visitClassStmt(ClassStmt<void, Value>* stmt) {
		environment->define(stmt->name, Null);
		Class* classDefinition = new Class(stmt->name.lexeme);
		generatedClasses.emplace_back(classDefinition);
		environment->assign(stmt->name, classDefinition);
	}

	void Interpreter::visitExpressionStmt(Expression<void, Value>* stmt) {
		evaluate(stmt->expression);
	}

	void Interpreter::visitFunctionStmt(FunctionStmt<void, Value>* stmt){
		ScriptCallable* function = new ScriptCallable(stmt, environment);
		generatedCallables.emplace_back(function);
		environment->define(stmt->name, function);
	}

	void Interpreter::visitIfStmt(If<void, Value>* stmt) {
		if (evaluate(stmt->condition).isTruthy())
			execute(stmt->thenBranch);
		else if (stmt->elseBranch != nullptr)
			execute(stmt->elseBranch);
	}

	void Interpreter::visitPrintStmt(Print<void, Value>* stmt) {
		Value val = evaluate(stmt->expression);
		printf("%s\n", val.toString().c_str());
	}
	
	void Interpreter::visitReturnStmt(Return<void, Value>* stmt) {
		Value val;
		if (stmt->value != nullptr)
			val = evaluate(stmt->value);

		throw new PittaReturn(val);
	}

	void Interpreter::visitVarStmt(Var<void, Value>* stmt) {
		Value value;
		if (stmt->initializer != nullptr) {
			value = evaluate(stmt->initializer);
		}

		environment->define(stmt->name, value);
	}

	void Interpreter::visitWhileStmt(While<void, Value>* stmt) {
		while (evaluate(stmt->condition).isTruthy()) {
			execute(stmt->body);
		}
	}

	void Interpreter::execute(Stmt<void, Value>* stmt) {
		stmt->accept(this);
	}


	class SetBack {
	public:
		SetBack(std::shared_ptr<Environment>& toSet, const std::shared_ptr<Environment>& value):
			toSet(toSet),
			value(value)
		{}

		~SetBack() {
			toSet = value;
		}
	private:
		std::shared_ptr<Environment>& toSet;
		std::shared_ptr<Environment> value;
	};

	void Interpreter::executeBlock(const std::vector<Stmt<void, Value>*>& statements, const std::shared_ptr<Environment>& newEnv) {
		std::shared_ptr<Environment> previous = environment;
		environment = newEnv;
		SetBack raiiTrySafe(environment, previous);

		try {
			for (Stmt<void, Value>* statement : statements)
				execute(statement);
		}
		catch (PittaRuntimeException* error) {
			throw(error);
		}
	}


	void Interpreter::interpret(Expr<Value>* expression) {
		try {
			printf("> %s\n", expression->accept(this).toString().c_str());
		}
		catch (...) {
			printf("Some heccin error occoured\n");
		}
	}

	void Interpreter::interpret(const std::vector<Stmt<void, Value>*>& statements) {
		try {
			for (Stmt<void, Value>* statement : statements)
				execute(statement);
		}
		catch (PittaRuntimeException* exception) {
			runtime->runtimeError(exception);
		}
	}

	Interpreter::Interpreter(Runtime* runtime):
		runtime(runtime),
		globals(std::make_shared<Environment>())
	{
		environment = globals;
	}


	Interpreter::Interpreter(Runtime* runtime, const std::shared_ptr<Environment>& globals):
		runtime(runtime),
		globals(globals)
	{
		environment = globals;
	}

	Interpreter::~Interpreter() {
		for (auto genFunc : generatedCallables)
			delete genFunc;
		for (auto genClass : generatedClasses)
			delete genClass;
		for (auto genInstance : generatedInstances)
			delete genInstance;
	}


	Runtime* Interpreter::getRuntime() {
		return runtime;
	}

	void Interpreter::resolve(Expr<Value>* expression, int depth) {
		locals.emplace(expression, depth);
	}

	Value Interpreter::lookUpVariable(const Token& name, Expr<Value>* expr) {
		if (locals.count(expr) == 0) {
			return globals->get(name);
		}
		else {
			int distance = locals.at(expr);
			return environment->getAt(distance, name.lexeme);
		}
	}

	void Interpreter::registerNewInstance(Instance* newInstance) {
		generatedInstances.emplace_back(newInstance);
	}

}