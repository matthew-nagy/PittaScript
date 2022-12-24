#include "PittaEnvironment.hpp"
#include "PittaHigherTypes.hpp"

namespace pitta {

	void Environment::define(const Token& name, const Value& value) {
		define(name.lexeme, value);
	}

	void Environment::define(const std::string& name, const Value& value) {
		values.emplace(name, value);
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

	Environment& Environment::operator|=(const Environment& other) {
		for (auto& pair : other.values)
			values.emplace(pair);

		return *this;
	}

	std::unordered_set<std::string> Environment::getDefinedValueNames()const {
		std::unordered_set<std::string> names;
		names.reserve(values.size());
		for (auto& [name, _] : values)
			names.emplace(name);
		return names;
	}
	std::unordered_map<std::string, Value> Environment::getDefinedValues(const std::unordered_set<std::string>& without) {
		std::unordered_map<std::string, Value> definedValues;
		values.reserve(values.size() - without.size());

		for (auto& [name, value] : values)
			if (without.count(name) == 0)
				definedValues.emplace(name, value);
		
		return definedValues;
	}


	Environment::Environment() :
		enclosing()
	{
		addTypeBindings(this);
	}

	Environment::Environment(const shared_data<Environment>& enclosing) :
		enclosing(enclosing)
	{}


	Environment* Environment::ancestor(int distance) {
		Environment* environment = this;
		for (size_t i = 0; i < distance; i++)
			environment = environment->enclosing.get();
		return environment;
	}
}