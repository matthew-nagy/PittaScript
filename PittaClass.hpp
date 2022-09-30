#pragma once
#include <vector>
#include "PittaValue.hpp"
#include "PittaFunction.hpp"
#include "PittaInterpreter.hpp"

namespace pitta {

	class Class : public Callable{
	public:
		const std::string name;

		std::string asString()const;

		int getArity()const override;

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override;

		Class(const std::string& name);
	};
	
	class Instance {
	public:

		std::string asString()const;

		const Value& get(const Token& name)const;
		const Value& get(const std::string& name)const;

		Instance(Class const* definition);
	private:
		Class const*const classDefinition;
		std::unordered_map<std::string, Value> fields;
	};
}