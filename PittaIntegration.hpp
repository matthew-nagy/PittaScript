#pragma once

#include "PittaClass.hpp"
#include "PittaFunction.hpp"
#include <functional>

namespace pitta {

	template<class T>
	class IntegratedInstance;
	template<class T>
	class IntegratedClass;

	template<class T>
	using IntegratedFunctionSigniture = std::function<Value(Interpreter*, const std::vector<Value>&, T*)>;

	template<class T>
	struct NameSigniturePair {
		IntegratedFunctionSigniture<T> signiture;
		int arity;

		NameSigniturePair(IntegratedFunctionSigniture<T> signiture, int arity) :
			signiture(signiture),
			arity(arity)
		{}

	};

	template<class T>
	using NewInstanceGenerator = T* (*)(Interpreter* interpreter, const std::vector<Value>& arguments);
	template<class T>
	using FieldsFromInstance = std::unordered_map<std::string, Value>(*)(T*);

	template<class T>
	class IntegratedClass : public Class {
	public:
		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override {
			T* newCppInstance = generateNewInstance(interpreter, arguments);
			auto fields = getFieldsFromInstance(newCppInstance);

			IntegratedInstance<T>* newInstance = new IntegratedInstance<T>(this, newCppInstance, fields);

			interpreter->registerNewInstance(newInstance);

			return newInstance;
		}

		Value bindExistingInstance(T* instance, Interpreter* interpreter) {
			auto fields = getFieldsFromInstance(instance);
			IntegratedInstance<T>* newInstance = new IntegratedInstance<T>(this, instance, fields);

			interpreter->registerNewInstance(newInstance);

			return newInstance;
		}

		IntegratedClass(const std::string& name, const std::unordered_map<std::string, Callable*>& methods, NewInstanceGenerator<T> generator, FieldsFromInstance<T> fielder):
			Class(name, nullptr, methods),
			generateNewInstance(generator),
			getFieldsFromInstance(fielder)
		{
			for (auto& [_, method] : methods)
				generatedCallables.emplace_back(method);
		}
		~IntegratedClass() {
			for (auto callable : generatedCallables)
				delete callable;
		}


	private:
		NewInstanceGenerator<T> generateNewInstance;
		FieldsFromInstance<T> getFieldsFromInstance;

		std::vector<Callable*> generatedCallables;
	};

	template<class T>
	class IntegratedInstance : public Instance {
	public:
		T* getInnerInstance()const {
			return instance;
		}

		IntegratedInstance(IntegratedClass<T> const* definition, T* instance, std::unordered_map<std::string, Value>& fields):
			Instance(definition),
			instance(instance)
		{
			this->fields = fields;
		}
	private:
		T* instance;
	};

	template<class T>
	class IntegratedCallable : public Callable {
	public:

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override {
			return function(interpreter, arguments, boundInstance->getInnerInstance());
		}

		Callable* bind(Instance* instance) {
			IntegratedCallable<T>* boundCallable = new IntegratedCallable<T>(getArity(), getName(), function);
			boundCallable->boundInstance = (IntegratedInstance<T>*)instance;
			return boundCallable;
		}

		IntegratedCallable(int arity, const std::string& name, IntegratedFunctionSigniture<T> sig):
			Callable(arity, name),
			function(sig)
		{}

	private:
		IntegratedFunctionSigniture<T> function;
		IntegratedInstance<T>* boundInstance;
	};

#define wrap(func, pittafunc) void pittafunc (Interpreter*, std::vector<Value>&){func();}
	class IT {
	public:

		void sayShortIntro() {
			innerShortIntro();
			printf("\n");
		}
		void sayFunFact() {
			printf("A fun fact about me is %s.\n", funFact.c_str());
		}
		void fullIntro() {
			innerShortIntro();
			printf(" ");
			sayFunFact();
		}

		void setFactoids(std::string inName, int inAge) {
			name = inName;
			age = inAge;
		}
		void setFunFact(std::string in) {
			funFact = in;
		}

		static std::unordered_map<std::string, Value> getPittaBinding(IT* instance) {
			std::unordered_map<std::string, Value> binding;

			binding.emplace("name", &instance->name);
			binding.emplace("age", &instance->age);
			binding.emplace("funFact", &instance->funFact);

			return binding;
		}
		static IT* generatePittaInstance(Interpreter* interpreter, const std::vector<Value>& arguments) {
			return new IT(arguments[0].asString(), arguments[1].asInt(), arguments[2].asString());
		}
		static std::unordered_map<std::string, Callable*> getPittaFunctions() {

			std::unordered_map<std::string, NameSigniturePair<IT>> funcMap;

			funcMap.try_emplace("sayShortIntro", NameSigniturePair(IntegratedFunctionSigniture<IT>([](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
				instance->sayShortIntro();
				return Null;
				}), 0 ));
			funcMap.try_emplace("sayFunFact", NameSigniturePair(IntegratedFunctionSigniture<IT>([](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
				instance->sayFunFact();
				return Null;
				}), 0));
			funcMap.try_emplace("fullIntro", NameSigniturePair(IntegratedFunctionSigniture<IT>([](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
				instance->fullIntro();
				return Null;
				}), 0));
			funcMap.try_emplace("setFactoids", NameSigniturePair(IntegratedFunctionSigniture<IT>([](Interpreter*, const std::vector<Value>& arguments, IT* instance)->Value {
				instance->setFactoids(arguments[0].asString(), arguments[1].asInt());
				return Null;
				}), 2));

			std::unordered_map<std::string, Callable*> f;
			for (auto& [name, p] : funcMap)
				f.emplace(name, new IntegratedCallable(p.arity, name, p.signiture));
			return f;
		}

		IT(std::string name, int age, std::string funFact):
			name(name),
			age(age),
			funFact(funFact)
		{}
	private:
		std::string name;
		int age;
		std::string funFact;

		void innerShortIntro() {
			printf("Hi! I'm %s, aged %d.", name.c_str(), age);
		}
	};

	IntegratedClass<IT>* ITClass = new IntegratedClass<IT>("IT", IT::getPittaFunctions(), IT::generatePittaInstance, IT::getPittaBinding);
}