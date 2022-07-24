#pragma once
#include "PittaExpressions.hpp"

namespace pitta {

    template<class T, class R>class Block;
    template<class T, class R>class Expression;
    template<class T, class R> class If;
    template<class T, class R>class Print;
    template<class T, class R>class Var;
    template<class T, class R>class While;

    template<class T, class R>
	class StatementVisitor {
    public:
        virtual T visitBlockStmt(Block<T, R>* stmt) = 0;
        //virtual T visitClassStmt(Class<T, R>* stmt) = 0;
        virtual T visitExpressionStmt(Expression<T, R>* stmt) = 0;
        //virtual T visitFunctionStmt(Function<T, R>* stmt) = 0;
        virtual T visitIfStmt(If<T, R>* stmt) = 0;
        virtual T visitPrintStmt(Print<T, R>* stmt) = 0;
        //virtual T visitReturnStmt(Return<T, R>* stmt) = 0;
        virtual T visitVarStmt(Var<T, R>* stmt) = 0;
        virtual T visitWhileStmt(While<T, R>* stmt) = 0;
	};

    template<class T, class R>
    class Stmt {
    public:
        virtual T accept(StatementVisitor<T, R>* visitor) = 0;
    };

    template<class T, class R>
    class Block : public Stmt<T, R> {
    public:
        std::vector<Stmt<T, R>*> statements;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitBlockStmt(this);
        }

        Block(const std::vector<Stmt<T, R>*>& statements) :
            statements(statements)
        {}

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
    class If : public Stmt<T, R> {
    public:
        Expr<R>* condition;
        Stmt<T, R>* thenBranch;
        Stmt<T, R>* elseBranch;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitIfStmt(this);
        }

        If(Expr<R>* condition, Stmt<T, R>* thenBranch, Stmt<T, R>* elseBranch) :
            condition(condition),
            thenBranch(thenBranch),
            elseBranch(elseBranch)
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

    template<class T, class R>
    class Var : public Stmt<T, R> {
    public:
        Token name;
        Expr<R>* initializer;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitVarStmt(this);
        }

        Var(const Token& name, Expr<R>* initializer) :
            name(name),
            initializer(initializer)
        {}

    };

    template<class T, class R>
    class While : public Stmt<T, R> {
    public:
        Expr<R>* condition;
        Stmt<T, R>* body;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitWhileStmt(this);
        }

        While(Expr<R>* condition, Stmt<T, R>* body) :
            condition(condition),
            body(body)
        {}

    };


    class StmtASTPrinter : public StatementVisitor<std::string, std::string> {
    public:
        std::string visitBlockStmt(Block<std::string, std::string>* stmt) {
            std::string ret = getTabbedOut() + "< block :";
            numOfTabsIn += 1;
            for (auto& s : stmt->statements) {
                ret += "\n" + s->accept(this);
            }
            numOfTabsIn -= 1;
            ret += "\n" + getTabbedOut() = ">";
            return ret;
        }
        //std::string visitClassStmt(Class<std::string, std::string>* stmt) = 0;
        std::string visitExpressionStmt(Expression<std::string, std::string>* stmt) {
            return getTabbedOut() + "<expr : " + stmt->expression->accept(&exprPrinter) + " >";
        }
        //std::string visitFunctionStmt(Function<std::string, std::string>* stmt) = 0;
        std::string visitIfStmt(If<std::string, std::string>* stmt) {
            return getTabbedOut() + "<if  " + stmt->condition->accept(&exprPrinter) + " : " + stmt->thenBranch->accept(this) +
                (stmt->elseBranch != nullptr ? (" else :" + stmt->elseBranch->accept(this)) : "") + " >";
        }
        std::string visitPrintStmt(Print<std::string, std::string>* stmt) {
            return getTabbedOut() + "<print : " + stmt->expression->accept(&exprPrinter) + " >";
        }
        //std::string visitReturnStmt(Return<std::string, std::string>* stmt) = 0;
        std::string visitVarStmt(Var<std::string, std::string>* stmt) {
            return getTabbedOut() + "<var : " + stmt->name.lexeme + (stmt->initializer == nullptr ? "" : (" : " + stmt->initializer->accept(&exprPrinter))) + " >";
        }
        std::string visitWhileStmt(While<std::string, std::string>* stmt) {
            std::string ret = getTabbedOut() + "<while : " + stmt->condition->accept(&exprPrinter);
            numOfTabsIn += 1;
            ret += "\n" + stmt->body->accept(this);
            numOfTabsIn -= 1;
            ret += "\n" + getTabbedOut() = ">";
            return ret;
        }
    private:
        ExprASTPrinter exprPrinter;
        int numOfTabsIn = 0;

        std::string getTabbedOut() {
            std::string tab = "";
            for (int i = 0; i < numOfTabsIn; i++)
                tab += "\t";
            return tab;
        }
    };

}