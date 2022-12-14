#pragma once
#include "PittaExpressions.hpp"

namespace pitta {

    template<class T, class R>class Block;
    template<class T, class R>class ClassStmt;
    template<class T, class R>class Expression;
    template<class T, class R>class FunctionStmt;
    template<class T, class R> class If;
    template<class T, class R>class Print;
    template<class T, class R>class Return;
    template<class T, class R>class Var;
    template<class T, class R>class While;

    template<class T, class R>
	class StatementVisitor {
    public:
        virtual T visitBlockStmt(Block<T, R>* stmt) = 0;
        virtual T visitClassStmt(ClassStmt<T, R>* stmt) = 0;
        virtual T visitExpressionStmt(Expression<T, R>* stmt) = 0;
        virtual T visitFunctionStmt(FunctionStmt<T, R>* stmt) = 0;
        virtual T visitIfStmt(If<T, R>* stmt) = 0;
        virtual T visitPrintStmt(Print<T, R>* stmt) = 0;
        virtual T visitReturnStmt(Return<T, R>* stmt) = 0;
        virtual T visitVarStmt(Var<T, R>* stmt) = 0;
        virtual T visitWhileStmt(While<T, R>* stmt) = 0;

        virtual ~StatementVisitor() = default;
	};

    template<class T, class R>
    class Stmt {
    public:
        virtual T accept(StatementVisitor<T, R>* visitor) = 0;

        virtual ~Stmt() = default;
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
    class ClassStmt : public Stmt<T, R> {
    public:
        Token name;
        Variable<R>* superclass;
        std::vector<FunctionStmt<T, R>*> methods;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitClassStmt(this);
        }

        ClassStmt(const Token& name, Variable<R>* superclass, std::vector<FunctionStmt<T, R>*>& methods):
            name(name),
            superclass(superclass),
            methods(methods)
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
    class FunctionStmt : public Stmt<T, R> {
    public:
        Token name;
        std::vector<Token> params;
        std::vector<Stmt<T, R>*> body;

        uint16_t variableId;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitFunctionStmt(this);
        }

        FunctionStmt(Token name, const std::vector<Token>& params, const std::vector<Stmt<T, R>*>& body):
            name(name),
            params(params),
            body(body)
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
    class Return : public Stmt<T, R> {
    public:
        Token keyword;
        Expr<R>* value;

        T accept(StatementVisitor<T, R>* visitor) {
            return visitor->visitReturnStmt(this);
        }

        Return(Token keyword, Expr<R>* value):
            keyword(keyword),
            value(value)
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
        std::string visitClassStmt(ClassStmt<std::string, std::string>* stmt) {
            return "Class: " + stmt->name.lexeme;
        }
        std::string visitExpressionStmt(Expression<std::string, std::string>* stmt) {
            return getTabbedOut() + "<expr : " + stmt->expression->accept(&exprPrinter) + " >";
        }
        std::string visitFunctionStmt(FunctionStmt<std::string, std::string>* stmt) {
            return "";
        }
        std::string visitIfStmt(If<std::string, std::string>* stmt) {
            return getTabbedOut() + "<if  " + stmt->condition->accept(&exprPrinter) + " : " + stmt->thenBranch->accept(this) +
                (stmt->elseBranch != nullptr ? (" else :" + stmt->elseBranch->accept(this)) : "") + " >";
        }
        std::string visitPrintStmt(Print<std::string, std::string>* stmt) {
            return getTabbedOut() + "<print : " + stmt->expression->accept(&exprPrinter) + " >";
        }
        std::string visitReturnStmt(Return<std::string, std::string>* stmt) {
            return "<return: " + stmt->value->accept(&exprPrinter) + ">";
        }
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