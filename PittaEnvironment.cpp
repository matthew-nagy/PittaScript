#include "PittaEnvironment.hpp"

namespace pitta {

	void Environment::define(const std::string& name, const Value value) {
		values.emplace(name, value);
	}

	void Environment::assign(const std::string& name, const Value& value) {
		if (values.count(name) > 0)
			values.at(name) = value;
		else if (enclosing != nullptr)
			enclosing->assign(name, value);
		else
			throw new PittaRuntimeException("Undefined variable '" + name + "'. Cannot assign.");
	}

	void Environment::assign(const Token& name, const Value& value) {
		assign(name.lexeme, value);
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

}