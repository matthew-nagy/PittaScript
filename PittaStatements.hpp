#pragma once
#include "PittaExpressions.hpp"

namespace pitta {

    template<class T, class R>class Expression;
    template<class T, class R>class Print;

    template<class T, class R>
	class StatementVisitor {
    public:
        //virtual T visitBlockStmt(Block<T, R>* stmt) = 0;
        //virtual T visitClassStmt(Class<T, R>* stmt) = 0;
        virtual T visitExpressionStmt(Expression<T, R>* stmt) = 0;
        //virtual T visitFunctionStmt(Function<T, R>* stmt) = 0;
        //virtual T visitIfStmt(If<T, R>* stmt) = 0;
        virtual T visitPrintStmt(Print<T, R>* stmt) = 0;
        //virtual T visitReturnStmt(Return<T, R>* stmt) = 0;
        //virtual T visitVarStmt(Var<T, R>* stmt) = 0;
        //virtual T visitWhileStmt(Whil<T, R>* e stmt) = 0;
	};

    template<class T, class R>
    class Stmt {
    public:
        virtual T accept(StatementVisitor<T, R>* visitor) = 0;
    };


    template<class T, class R>
    class Expression : public Stmt<T, R> {
    public:
        Expr<R>* expression;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitExpressionStmt(this);
        }

        Expression(Expr<R>* expression):
            expression(expression)
        {}

    };

    template<class T, class R>
    class Print : public Stmt<T, R> {
    public:
        Expr<R>* expression;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitPrintStmt(this);
        }

        Print(Expr<R>* expression) :
            expression(expression)
        {}

    };

}