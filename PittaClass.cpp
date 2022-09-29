#include "PittaClass.hpp"

namespace pitta {

	std::string Class::asString()const {
		return name;
	}

	Value Class::operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const {
		Instance* newInstance = new Instance(this);

		interpreter->registerNewInstance(newInstance);

		return newInstance;
	}

	int Class::getArity()const {
		return 0;
	}

	Class::Class(const std::string& name) :
		Callable(0, name),
		name(name)
	{}



	std::string Instance::asString()const {
		return "Instance of class " + classDefinition->getName();
	}

	Instance::Instance(Class const* definition) :
		classDefinition(definition)
	{}


}