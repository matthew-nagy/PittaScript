#pragma once
#include <vector>
#include "PittaValue.hpp"
#include "PittaFunction.hpp"
#include "PittaInterpreter.hpp"

namespace pitta {

	class Class : public Callable{
	public:
		const std::string name;
		Class const*const superclass;

		std::string asString()const;

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments)const override;

		Callable* findMethod(const std::string& methodName)const;

		Class(const std::string& name, Class const* superclass, std::unordered_map<std::string, Callable*>&& methods);
		Class(const std::string& name, Class const* superclass, const std::unordered_map<std::string, Callable*>& methods);
		virtual ~Class() = default;
	protected:
		std::unordered_map<std::string, Callable*> methods;
	};
	
	class Instance {
	public:

		std::string asString()const;

		Value get(const Token& name);
		Value get(const std::string& name);

		void set(const Token& name, const Value& values);
		void set(const std::string& name, const Value& value);

		Instance(Class const* definition);
		virtual ~Instance();
	protected:
		Class const*const classDefinition;
		std::unordered_map<std::string, Value> fields;
		std::vector<Callable*> generatedCallables;
	};
}