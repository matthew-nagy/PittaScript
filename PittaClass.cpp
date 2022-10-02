#include "PittaClass.hpp"

namespace pitta {

	std::string Class::asString()const {
		return name;
	}

	Callable* Class::findMethod(const std::string& methodName)const {
		if (methods.count(methodName) > 0)
			return methods.at(methodName);
		return nullptr;
	}

	Value Class::operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const {
		Instance* newInstance = new Instance(this);

		interpreter->registerNewInstance(newInstance);

		Callable* initaliser = findMethod("init");
		if (initaliser != nullptr) {
			Callable* boundInit = initaliser->bind(newInstance);
			(*boundInit)(interpreter, arguments);
			delete boundInit;
		}

		return newInstance;
	}

	Class::Class(const std::string& name, std::unordered_map<std::string, Callable*>&& methods) :
		Callable(findArity(methods), name),
		name(name),
		methods(std::move(methods))
	{}

	int Class::findArity(const std::unordered_map<std::string, Callable*>& methods)const {
		if (methods.count("init") == 0)
			return 0;
		return methods.at("init")->getArity();
	}


	std::string Instance::asString()const {
		return "Instance of class " + classDefinition->getName();
	}

	Value Instance::get(const Token& name) {
		return get(name.lexeme);
	}
	Value Instance::get(const std::string& name) {
		if (fields.count(name) > 0) {
			return fields.at(name);
		}
		Callable* method = classDefinition->findMethod(name);
		if (method != nullptr) {
			Callable* newMethod = method->bind(this);
			generatedCallables.emplace_back(newMethod);
			return newMethod;
		}

		throw new PittaRuntimeException("Cannot find field with name " + name + ".");
		return Undefined;
	}

	void Instance::set(const Token& name, const Value& value) {
		set(name.lexeme, value);
	}
	void Instance::set(const std::string& name, const Value& value) {
		fields.emplace(name, value);
	}

	Instance::Instance(Class const* definition) :
		classDefinition(definition)
	{}
	Instance::~Instance() {
		for (auto callable : generatedCallables)
			delete callable;
	}


}