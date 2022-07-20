#pragma once
#include "PittaExpressions.hpp"

namespace pitta {

    template<class T>class Print;

    template<class T>
	class StatementVisitor {
        //virtual T visitBlockStmt(Block<T>* stmt) = 0;
        //virtual T visitClassStmt(Class<T>* stmt) = 0;
        //virtual T visitExpressionStmt(Expression<T>* stmt) = 0;
        //virtual T visitFunctionStmt(Function<T>* stmt) = 0;
        //virtual T visitIfStmt(If<T>* stmt) = 0;
        virtual T visitPrintStmt(Print<T>* stmt) = 0;
        //virtual T visitReturnStmt(Return<T>* stmt) = 0;
        //virtual T visitVarStmt(Var<T>* stmt) = 0;
        //virtual T visitWhileStmt(Whil<T>* e stmt) = 0;
	};

    template<class T>
    class Stmt {
    public:
        virtual T accept(StatementVisitor<T>* visitor) = 0;
    };


    template<class T>
    class Print : public Stmt<T> {
    public:
        Expr<T>* expression;

        T accept(StatementVisitor<T>* visitor) {
            return visitor->visitPrintStmt(this);
        }

        Print(Expr<T>* expression):
            expression(expression)
        {}

    };

}