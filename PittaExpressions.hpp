#pragma once
#include "PittaTokenScanner.hpp"

namespace pitta {

	template<class R> class Literal;
	template<class R> class Assign;
	template<class R> class Binary;
	template<class R> class Grouping;
	template<class R> class Unary;
	template<class R> class Variable;
	template<class R> class Logical;
	template<class R> class Call;
	template<class R> class Get;
	template<class R> class Set;
	template<class R> class This;
	template<class R> class Super;

	template<class T>
	class ExpressionVisitor {
	public:
		virtual ~ExpressionVisitor() = default;
		virtual T visitLiteralExpr(Literal<T>* expr) = 0;
		//virtual T visitAssignExpr(Assign<T>* expr) = 0;
		virtual T visitBinaryExpr(Binary<T>* expr) = 0;
		virtual T visitGroupingExpr(Grouping<T>* expr) = 0;
		virtual T visitUnaryExpr(Unary<T>* expr) = 0;
		//virtual T visitVariableExpr(Variable<T>* expr) = 0;
		//virtual T visitLogicalExpr(Logical<T>* expr) = 0;
		//virtual T visitCallExpr(Call<T>* expr) = 0;
		//virtual T visitGetExpr(Get<T>* expr) = 0;
		//virtual T visitSetExpr(Set<T>* expr) = 0;
		//virtual T visitThisExpr(This<T>* expr) = 0;
		//virtual T visitSuperExpr(Super<T>* expr) = 0;
	};


	template<class T>
	class Expr {
	public:
		virtual T accept(ExpressionVisitor<T>* visitor) = 0;
	};

#define SingleArgExp(Name, ArgQualifier, ArgName, visitType)\
template<class T>\
class Name : public Expr<T>{\
public:\
	ArgQualifier ArgName;\
	T accept(ExpressionVisitor<T>* visitor)override {\
		return visitor-> visitType (this);\
	}\
	Name ( ArgQualifier ArgName ):\
		ArgName(ArgName)\
	{}\
}\

#define DoubleArgExp(Name, ArgQualifier1, ArgName1, ArgQualifier2, ArgName2, visitType)\
template<class T>\
class Name : public Expr<T>{\
public:\
	ArgQualifier1 ArgName1;\
	ArgQualifier2 ArgName2;\
	T accept(ExpressionVisitor<T>* visitor)override {\
		return visitor-> visitType (this);\
	}\
	Name ( ArgQualifier1 ArgName1, ArgQualifier2 ArgName2 ):\
		ArgName1(ArgName1),\
		ArgName2(ArgName2)\
	{}\
}\

#define TripleArgExp(Name, ArgQualifier1, ArgName1, ArgQualifier2, ArgName2, ArgQualifier3, ArgName3, visitType)\
template<class T>\
class Name : public Expr<T>{\
public:\
	ArgQualifier1 ArgName1;\
	ArgQualifier2 ArgName2;\
	ArgQualifier3 ArgName3;\
	T accept(ExpressionVisitor<T>* visitor)override {\
		return visitor-> visitType (this);\
	}\
	Name ( ArgQualifier1 ArgName1, ArgQualifier2 ArgName2, ArgQualifier3 ArgName3 ):\
		ArgName1(ArgName1),\
		ArgName2(ArgName2),\
		ArgName3(ArgName3)\
	{}\
}\

	TripleArgExp(Binary, Expr<T>*, left, Token, op, Expr<T>*, right, visitBinaryExpr);

	SingleArgExp(Grouping, Expr<T>*, expression, visitGroupingExpr);

	SingleArgExp(Literal, Value, value, visitLiteralExpr);

	DoubleArgExp(Unary, Token, op, Expr<T>*, right, visitUnaryExpr);


	class ASTPrinter : public ExpressionVisitor<std::string> {
	public:
		std::string print(Expr<std::string>* expr) {
			return expr->accept(this);
		}

		std::string visitBinaryExpr(Binary<std::string>* expr) {
			const std::string ret = "( " + expr->op.lexeme + " " + expr->left->accept(this) + " " + expr->right->accept(this) + " )";
			//printf("Visiting binary expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitGroupingExpr(Grouping<std::string>* expr) {
			const std::string ret = "( group" + expr->expression->accept(this) + " )";
			//printf("Visiting grouping expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitLiteralExpr(Literal<std::string>* expr) {
			const std::string ret = "( " + expr->value.toString() + " )";
			//printf("Visiting literal expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitUnaryExpr(Unary<std::string>* expr) {
			const std::string ret = "( " + expr->op.lexeme + " " + expr->right->accept(this) + " )";
			//printf("Visiting unary expression: %s\n", ret.c_str());
			return ret;
		}
	};

}