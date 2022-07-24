#pragma once
#include <unordered_map>
#include <memory>
#include "PittaValue.hpp"
#include "PittaTokenScanner.hpp"

namespace pitta {

	class Environment{
	public:

		std::shared_ptr<Environment> enclosing;

		void define(const std::string& name, const Value value);

		void assign(const std::string& name, const Value& value);

		void assign(const Token& name, const Value& value);

		Value get(const std::string& name);
		Value get(const Token& token);

		Environment();
		
		Environment(const std::shared_ptr<Environment>& enclosing);

	private:

		std::unordered_map<std::string, Value> values;

	};

}