#include "PittaEnvironment.hpp"

namespace pitta {

	void Environment::define(const Token& name, const Value& value) {
		values.emplace(name.lexeme, value);
	}

	void Environment::assign(const Token& name, const Value& value) {
		assign(name.lexeme, value);
	}

	void Environment::assign(const std::string& name, const Value& value) {
		values[name] = value;
	}

	void Environment::assignAt(int distance, const Token& name, const Value& value) {
		ancestor(distance)->assign(name, value);
	}

	Value Environment::getAt(int distance, const std::string& name) {
		return ancestor(distance)->get(name);
	}


	Value Environment::get(const std::string& name) {
		if (values.count(name) > 0)
			return values.at(name);

		if (enclosing != nullptr)
			return enclosing->get(name);

		throw new PittaRuntimeException("Undefined variable '" + name + "'. Cannot read value.");
	}
	Value Environment::get(const Token& token) {
		return get(token.lexeme);
	}

	Environment::Environment() :
		enclosing(nullptr)
	{}

	Environment::Environment(const std::shared_ptr<Environment>& enclosing) :
		enclosing(enclosing)
	{}


	Environment* Environment::ancestor(int distance) {
		Environment* environment = this;
		for (size_t i = 0; i < distance; i++)
			environment = environment->enclosing.get();
		return environment;
	}
}