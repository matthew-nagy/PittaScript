#pragma once
#include <unordered_map>
#include "PittaValue.hpp"
#include "PittaTokenScanner.hpp"
#include <stdint.h>
#include <unordered_set>

namespace pitta {

	class Environment{
	public:

		shared_data<Environment> enclosing;

		void define(const Token& name, const Value& value);

		void define(const std::string& name, const Value& value);

		void assign(const Token& name, const Value& value);

		void assign(const std::string& name, const Value& value);

		void assignAt(int distance, const Token& name, const Value& value);


		Value get(const std::string& name);
		Value get(const Token& token);


		Value getAt(int distance, const std::string& name);

		std::unordered_set<std::string> getDefinedValueNames()const;
		std::unordered_map<std::string, Value> getDefinedValues(const std::unordered_set<std::string>& without = {});


		Environment();
		
		Environment(const shared_data<Environment>& enclosing);

	private:

		std::unordered_map<std::string, Value> values;

		Environment* ancestor(int distance);

	};

}