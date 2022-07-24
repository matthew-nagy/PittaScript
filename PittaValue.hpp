#pragma once
#include <string>
#include <stdexcept>
#include <unordered_map>

namespace pitta {

	class PittaRuntimeException final : public std::runtime_error {
	public:
		std::string details;

		const char* what()const noexcept override;

		PittaRuntimeException(const std::string& detail);
	};

	enum Type {
		Int, Float, String, Bool, Null, Undefined, Instance, Class, Function
	};

	extern const std::unordered_map<Type, std::string> c_typeToString;

	class Callable;

	std::string getSubstring(const std::string& from, int startIndex, int endIndex);

	class Value {
	public:
		Type getType()const;
		bool isBoundValue()const;

		std::string toString()const;

		bool isTruthy()const;

		int asInt()const;
		float asFloat()const;
		bool asBool()const;
		std::string asString()const;
		const Callable* asCallable()const;

		void setInt(int value);
		void setFloat(float value);
		void setBool(bool value);
		void setString(const std::string& value);
		void setCallable(const Callable* callable);
		void setNull();
		void setUndefined();

		void bindInt(int* toBind);
		void bindFloat(float* toBind);
		void bindBool(bool* toBind);
		void bindString(std::string* toBind);

		Value& operator=(const Value& right);
		Value& operator=(int value);
		Value& operator=(float value);
		Value& operator=(bool value);
		Value& operator=(const std::string& value);
		Value& operator=(const Callable* callable);
		Value& operator=(Type);

		bool operator==(const Value& right)const;

		Value() = default;
		Value(int val);
		Value(float val);
		Value(bool val);
		Value(const std::string & val);
		Value(int* val);
		Value(float* val);
		Value(bool* val);
		Value(std::string* val);
		Value(const Callable* callable);

	private:
		Type type = Undefined;
		bool isBoundPointer = false;

		union {
			int intVal, * intValP;
			float floatVal, * floatValP;
			bool boolVal, * boolValP;
			std::string* stringValP;
			const Callable* func;
		} rep;
		std::string stringVal;
	};

}

typedef pitta::Value PittaValue;