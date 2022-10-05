#pragma once
#include "PittaTokenScanner.hpp"
#include <typeindex>

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
		virtual T visitLiteralExpr(Literal<T>* expr) = 0;
		virtual T visitAssignExpr(Assign<T>* expr) = 0;
		virtual T visitBinaryExpr(Binary<T>* expr) = 0;
		virtual T visitGroupingExpr(Grouping<T>* expr) = 0;
		virtual T visitUnaryExpr(Unary<T>* expr) = 0;
		virtual T visitVariableExpr(Variable<T>* expr) = 0;
		virtual T visitLogicalExpr(Logical<T>* expr) = 0;
		virtual T visitCallExpr(Call<T>* expr) = 0;
		virtual T visitGetExpr(Get<T>* expr) = 0;
		virtual T visitSetExpr(Set<T>* expr) = 0;
		virtual T visitThisExpr(This<T>* expr) = 0;
		virtual T visitSuperExpr(Super<T>* expr) = 0;

		virtual ~ExpressionVisitor() = default;
	};


	template<class T>
	class Expr {
	public:
		virtual T accept(ExpressionVisitor<T>* visitor) = 0;

		virtual std::type_index getType()const = 0;

		virtual ~Expr() = default;
	};

#define SingleArgExp(Name, ArgQualifier, ArgName, visitType)\
template<class T>\
class Name : public Expr<T>{\
public:\
	ArgQualifier ArgName;\
	T accept(ExpressionVisitor<T>* visitor)override {\
		return visitor-> visitType (this);\
	}\
	std::type_index getType()const override{return typeid( Name );}\
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
	std::type_index getType()const override{return typeid( Name );}\
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
	std::type_index getType()const override{return typeid( Name );}\
	Name ( ArgQualifier1 ArgName1, ArgQualifier2 ArgName2, ArgQualifier3 ArgName3 ):\
		ArgName1(ArgName1),\
		ArgName2(ArgName2),\
		ArgName3(ArgName3)\
	{}\
}\


	template<class T>
	class Assign : public Expr<T> {
	public:
		Token name;
		Expr<T>* value;

		uint16_t environmentDepth;
		uint16_t variableId;

		T accept(ExpressionVisitor<T>* visitor)override {
			return visitor->visitAssignExpr(this);
		}
		std::type_index getType()const override { return typeid(Assign); }
		Assign(Token name, Expr<T>* value) :
			name(name),
			value(value)
		{}
	};

	template<class T>
	class Binary : public Expr<T> {
	public:
		Expr<T>* left;
		Token op;
		Expr<T>* right;
		T accept(ExpressionVisitor<T>* visitor)override {
			return visitor->visitBinaryExpr(this);
		}
		std::type_index getType()const override {
			return typeid(Binary);
		}
		Binary(Expr<T>* left, Token op, Expr<T>* right) :
			left(left),
			op(op),
			right(right)
		{}
	};

	TripleArgExp(Call, Expr<T>*, callee, Token, closingParenthesis, std::vector<Expr<T>*>, arguments, visitCallExpr);

	DoubleArgExp(Get, Expr<T>*, object, Token, name, visitGetExpr);

	SingleArgExp(Grouping, Expr<T>*, expression, visitGroupingExpr);

	SingleArgExp(Literal, Value, value, visitLiteralExpr);

	TripleArgExp(Logical, Expr<T>*, left, Token, op, Expr<T>*, right, visitLogicalExpr);

	TripleArgExp(Set, Expr<T>*, object, Token, name, Expr<T>*, value, visitSetExpr);

	DoubleArgExp(Super, Token, keyword, Token, method, visitSuperExpr);

	SingleArgExp(This, Token, keyword, visitThisExpr);

	DoubleArgExp(Unary, Token, op, Expr<T>*, right, visitUnaryExpr);

	template<class T>
	class Variable : public Expr<T> {
	public:
		Token name;

		uint16_t environmentDepth;

		T accept(ExpressionVisitor<T>* visitor)override {
			return visitor->visitVariableExpr(this);
		}
		std::type_index getType()const override { return typeid(Variable); }
		Variable(Token name) :
			name(name)
		{}
	};


	class ExprASTPrinter : public ExpressionVisitor<std::string> {
	public:
		std::string print(Expr<std::string>* expr) {
			return expr->accept(this);
		}

		std::string visitAssignExpr(Assign<std::string>* expr) override{
			return "( " + expr->name.lexeme + " = " + expr->value->accept(this) + " )";
		}

		std::string visitBinaryExpr(Binary<std::string>* expr) override{
			const std::string ret = "( " + expr->op.lexeme + " " + expr->left->accept(this) + " " + expr->right->accept(this) + " )";
			//printf("Visiting binary expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitCallExpr(Call<std::string>* expr) override{
			std::string ret = "( call function on " + expr->callee->accept(this) + "with args (";
			for (Expr<std::string>* arg : expr->arguments)
				ret += arg->accept(this) + ", ";
			return ret.substr(0, ret.size() - 2) + ") )";
		}

		std::string visitGetExpr(Get<std::string>* expr) override{
			throw 0;
			//TODO
			const std::string ret = "";
			return ret;
		}

		std::string visitGroupingExpr(Grouping<std::string>* expr) override{
			const std::string ret = "( group" + expr->expression->accept(this) + " )";
			//printf("Visiting grouping expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitLiteralExpr(Literal<std::string>* expr) override{
			const std::string ret = "( " + expr->value.toString() + " )";
			//printf("Visiting literal expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitLogicalExpr(Logical<std::string>* expr) override{
			const std::string ret = "( " + expr->left->accept(this) + " " + expr->op.lexeme + " " + expr->left->accept(this) + " )";
			//printf("Visiting literal expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitSetExpr(Set<std::string>* expr) override{
			throw 0;
			//TODO
			const std::string ret = "";
			return ret;
		}

		std::string visitSuperExpr(Super<std::string>* expr) override{
			throw 0;
			//TODO
			const std::string ret = "";
			return ret;
		}

		std::string visitThisExpr(This<std::string>* expr) override{
			throw 0;
			//TODO
			const std::string ret = "";
			return ret;
		}

		std::string visitUnaryExpr(Unary<std::string>* expr) override{
			const std::string ret = "( " + expr->op.lexeme + " " + expr->right->accept(this) + " )";
			//printf("Visiting unary expression: %s\n", ret.c_str());
			return ret;
		}

		std::string visitVariableExpr(Variable<std::string>* expr) override{
			const std::string ret = "( var '" + expr->name.lexeme + "' )";
			//printf("Visiting unary expression: %s\n", ret.c_str());
			return ret;
		}
	};

}