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

		Value bindExistingInstance(T* instance) {
			auto fields = getFieldsFromInstance(instance);
			IntegratedInstance<T>* newInstance = new IntegratedInstance<T>(this, instance, fields);

			generatedInstances.emplace_back(newInstance);

			return newInstance;
		}

		int getArity()const override {
			return 3;
		}

		IntegratedClass(const std::string& name, const std::unordered_map<std::string, Callable*>& methods, int generatorArity, NewInstanceGenerator<T> generator, FieldsFromInstance<T> fielder):
			Class(name, nullptr, methods),
			generateNewInstance(generator),
			getFieldsFromInstance(fielder)
		{
			for (auto& [_, method] : methods)
				generatedCallables.emplace_back(method);
			
//			Callable* initCallable = new NativeCallable(generatorArity, generateNewInstance);
			//arity = generatorArity;
			//generatedCallables.emplace_back(initCallable);
			//this->methods.emplace("init", initCallable);
		}
		~IntegratedClass() {
			for (auto callable : generatedCallables)
				delete callable;
			for (auto instance : generatedInstances)
				delete instance;
		}


	private:
		NewInstanceGenerator<T> generateNewInstance;
		FieldsFromInstance<T> getFieldsFromInstance;

		std::vector<Callable*> generatedCallables;
		//Those made before the interpreter is created
		std::vector<Instance*> generatedInstances;
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

		class Binder {
		public:
			std::unordered_map<std::string, Callable*> get()const {
				return callables;
			}

			template<class Lambda>
			Binder& add(const std::string& name, int arity, Lambda lambda) {
				callables.try_emplace(name, new IntegratedCallable(arity, name, IntegratedFunctionSigniture<T>(lambda)));
				return *this;
			}

		private:
			std::unordered_map<std::string, Callable*> callables;
		};

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override {
			return function(interpreter, arguments, boundInstance->getInnerInstance());
		}

		Callable* bind(Instance* instance) override {
			IntegratedCallable<T>* boundCallable = new IntegratedCallable<T>(getArity(), getName(), function);
			boundCallable->boundInstance = (IntegratedInstance<T>*)instance;
			return boundCallable;
		}

		IntegratedCallable(int arity, const std::string& name, IntegratedFunctionSigniture<T> sig):
			Callable(arity, name),
			function(sig),
			boundInstance(nullptr)
		{}

	private:
		IntegratedFunctionSigniture<T> function;
		IntegratedInstance<T>* boundInstance;
	};



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

		void doubleCheck() {
			printf("I am at %p\n", this);
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
			return IntegratedCallable<IT>::Binder()
				.add("sayShortIntro", 0, [](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
				instance->sayShortIntro();
				return Null;
					})
				.add("sayFunFact", 0, [](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
						instance->sayShortIntro();
						return Null;
					})
				.add("fullIntro", 0, [](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
						instance->fullIntro();
						return Null;
					})
				.add("dbc", 0, [](Interpreter*, const std::vector<Value>&, IT* instance)->Value {
						instance->doubleCheck();
						return Null;
					})
				.add("setFactoids", 2, [](Interpreter*, const std::vector<Value>& arguments, IT* instance)->Value {
						instance->setFactoids(arguments[0].asString(), arguments[1].asInt());
						return Null;
					})
				.get();
		}

		IT(std::string name, int age, std::string funFact):
			name(name),
			age(age),
			funFact(funFact)
		{
			printf("Created at %p\n", this);
		}
	private:
		std::string name;
		int age;
		std::string funFact;

		void innerShortIntro() {
			printf("Hi! I'm %s, aged %d.", name.c_str(), age);
		}
	};

	IntegratedClass<IT>* ITClass = new IntegratedClass<IT>("IT", IT::getPittaFunctions(), 3, IT::generatePittaInstance, IT::getPittaBinding);
}