#pragma once

#include "PittaValue.hpp"
#include "PittaInterpreter.hpp"


namespace pitta {

	typedef Value(*FunctionSigniture)(Interpreter*, const std::vector<Value>&);
	class Instance;

	class Callable {
	public:

		virtual int getArity()const {
			return arity;
		}

		const std::string& getName()const{
			return name;
		}
		virtual Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const = 0;

		virtual Callable* bind(Instance* instance) {
			throw new PittaRuntimeException("This type of function cannot be bound.");
			return nullptr;
		}

		Callable(int arity, const std::string& name):
			arity(arity),
			name(name)
		{}
		virtual ~Callable() = default;
	protected:
		int arity;
		const std::string name;
	};

	class NativeCallable : public Callable{
	public:

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override {
			return func(interpreter, arguments);
		}

		NativeCallable(int arity, FunctionSigniture function) :
			Callable(arity, "Unnamed native function"),
			func(function)
		{}

		NativeCallable(int arity, const std::string& name, FunctionSigniture function) :
			Callable(arity, name),
			func(function)
		{}
	private:
		FunctionSigniture func;
	};

	class ScriptCallable : public Callable {
	public:
		FunctionStmt<void, Value>* declaration;
		shared_data<Environment> closure;

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override {
			shared_data<Environment> environment = make_shared_data<Environment>(closure);
			for (size_t i = 0; i < arguments.size(); i++)
				environment->define(declaration->params[i], arguments[i]);

			Value toReturn;

			try {
				interpreter->executeBlock(declaration->body, environment);
			}
			catch (PittaReturn* returnValue) {
				toReturn = returnValue->value;
				delete returnValue;
			}
			return toReturn;
		}

		Callable* bind(Instance* instance) override{
			shared_data<Environment> environment = make_shared_data<Environment>(closure);
			environment->define(c_classSelfReferenceKey, instance);
			
			return new ScriptCallable(declaration, environment);
		}

		ScriptCallable(FunctionStmt<void, Value>* declaration, shared_data<Environment> closure):
			Callable(declaration->params.size(), declaration->name.lexeme),
			declaration(declaration),
			closure(closure)
		{}
	};
}