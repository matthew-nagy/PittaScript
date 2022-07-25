#pragma once
#include <unordered_map>
#include <memory>
#include "PittaValue.hpp"
#include "PittaTokenScanner.hpp"
#include <stdint.h>

namespace pitta {

	class Environment{
	public:

		std::shared_ptr<Environment> enclosing;

		void define(uint16_t id, const Value value);

		void assign(uint16_t id, const Value& value);

		void assign(uint16_t id, uint16_t depth, const Value& value);

		//void assign(const Token& name, const Value& value);

		//Value get(const std::string& name);
		//Value get(const Token& token);
		Value get(uint16_t index);

		Environment();
		
		Environment(const std::shared_ptr<Environment>& enclosing);

	private:

		std::unordered_map<uint16_t, Value> values;

	};

}