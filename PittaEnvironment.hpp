#pragma once
#include <unordered_map>
#include <memory>
#include "PittaValue.hpp"
#include "PittaTokenScanner.hpp"

namespace pitta {

	class Environment{
	public:

		std::shared_ptr<Environment> enclosing;

		void define(const std::string& name, const Value value) {
			values.emplace(name, value);
		}

		void assign(const std::string& name, const Value& value) {
			if (values.count(name) > 0)
				values.at(name) = value;
			if (enclosing != nullptr)
				enclosing->assign(name, value);

			throw new PittaRuntimeException("Undefined variable '" + name + "'.");
		}

		void assign(const Token& name, const Value& value) {
			assign(name.lexeme, value);
		}

		Value get(const std::string& name) {
			if (values.count(name) > 0)
				return values.at(name);

			if (enclosing != nullptr)
				return enclosing->get(name);

			throw new PittaRuntimeException("Undefined variable '" + name + "'.");
		}
		Value get(const Token& token) {
			return get(token.lexeme);
		}

		Environment():
			enclosing(nullptr)
		{}
		
		Environment(const std::shared_ptr<Environment>& enclosing):
			enclosing(enclosing)
		{}

	private:

		std::unordered_map<std::string, Value> values;

	};

}