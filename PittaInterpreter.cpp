#include "PittaInterpreter.hpp"
#include "PittaRuntime.hpp"

namespace pitta {

	Value Interpreter::visitBinaryExpr(Binary<Value>* expr) {
		Value left = evaluate(expr->left);
		Value right = evaluate(expr->right);
		Value result;

		const std::string invalidTypeMsg = "Invalid numeric types for operator";

#define Numeric_Maths_Switch(OpType, Symbol)\
			case OpType:\
				switch (left.getType()) {\
				case Int:\
					return result = (left.asInt() Symbol right.asInt());\
				case Float:\
					return result = (left.asFloat() Symbol right.asFloat());\
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
		return result = (left.asInt() Symbol right.asInt());\
	break

		switch (expr->op.type) {
			Numeric_Maths_Switch(MINUS, -);
			Numeric_Maths_Switch(PLUS, +);
			Numeric_Maths_Switch(SLASH, / );
			Numeric_Maths_Switch(STAR, *);

		case EQUAL_EQUAL:
			return result = (left == right);
		case BANG_EQUAL:
			return result = !(left == right);
			Numeric_Maths_Switch(LESS, < );
			Numeric_Maths_Switch(GREATER, > );
			Numeric_Maths_Switch(LESS_EQUAL, <= );
			Numeric_Maths_Switch(GREATER_EQUAL, >= );

		case SHIFT_LEFT:
			printf("%d << %d = %d\n", left.asInt(), right.asInt(), left.asInt() << right.asInt());
			return result = (left.asInt() << right.asInt());
		case SHIFT_RIGHT:
			return result = (left.asInt() >> right.asInt());

		case STRING_CONCAT:
			if (left.getType() != String || right.getType() != String) {
				runtime->error(expr->op, "Non string values cannot undergo string style concatination");
				throw new PittaRuntimeException("Non string values cannot undergo string style concatination");
			}
			else {
				return result = (left.asString() + right.asString());
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

	Value Interpreter::visitGroupingExpr(Grouping<Value>* expr) {
		return evaluate(expr->expression);
	}

	Value Interpreter::visitLiteralExpr(Literal<Value>* expr) {
		return expr->value;
	}

	Value Interpreter::visitUnaryExpr(Unary<Value>* expr) {
		Value right = evaluate(expr->right);
		Value toReturn;

		switch (expr->op.type) {
		case MINUS:
			switch (right.getType()) {
			case Int:
				return toReturn = (-1 * right.asInt());
			case Float:
				return toReturn = (-1.0f * right.asFloat());
			default:
				runtime->error(expr->op, "Negation must be followed by and integer or floating point value");
				throw new PittaRuntimeException("Negation must be followed by and integer or floating point value");
			}
			break;
		case BANG:
			return toReturn = !right.isTruthy();
		case BIT_NOT:
			if (right.getType() == Int) {
				return toReturn = ~right.asInt();
			}
			else {
				runtime->error(expr->op, "Logical not operator may only be applied to integer values");
				throw new PittaRuntimeException("Logical not operator may only be applied to integer values");
			}
		}

		return toReturn = Undefined;
	}

	Value Interpreter::evaluate(Expr<Value>* expression) {
		return expression->accept(this);
	}

	void Interpreter::visitExpressionStmt(Expression<void, Value>* stmt) {
		evaluate(stmt->expression);
	}

	void Interpreter::visitPrintStmt(Print<void, Value>* stmt) {
		Value val = evaluate(stmt->expression);
		printf("%s\n", val.toString());
	}


	void Interpreter::execute(Stmt<void, Value>* stmt) {
		stmt->accept(this);
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

	Interpreter::Interpreter(Runtime* runtime) :
		runtime(runtime)
	{}

}