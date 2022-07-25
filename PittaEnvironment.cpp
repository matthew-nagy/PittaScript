#include "PittaEnvironment.hpp"

namespace pitta {

	void Environment::define(uint16_t id, const Value value) {
		values.emplace(id, value);
	}

	void Environment::assign(uint16_t id, const Value& value) {
		if (values.count(id) > 0)
			values.at(id) = value;
		else if (enclosing != nullptr)
			enclosing->assign(id, value);
		else
			throw new PittaRuntimeException("Undefined variable '" + std::to_string(id) + "'. Cannot assign.");
	}

	void Environment::assign(uint16_t id, uint16_t depth, const Value& value) {
		if (depth == 0)
			assign(id, value);
		else
			enclosing->assign(id, depth - 1, value);
	}

	Value Environment::get(uint16_t index){
		if (values.count(index) > 0)
			return values.at(index);

		if (enclosing != nullptr)
			return enclosing->get(index);

		throw new PittaRuntimeException("Undefined variable '" + std::to_string(index) + "'. Cannot read value.");
	}

	/*
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
	}*/

	Environment::Environment() :
		enclosing(nullptr)
	{}

	Environment::Environment(const std::shared_ptr<Environment>& enclosing) :
		enclosing(enclosing)
	{}

}