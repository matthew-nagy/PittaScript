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

	std::string getSubstring(const std::string& from, int startIndex, int endIndex);

	class Value {
	public:
		Type getType()const;
		bool isBoundValue()const;

		std::string toString()const;

		bool isTruthy()const;

		const int& asInt()const;
		const float& asFloat()const;
		const bool& asBool()const;
		const std::string& asString()const;

		void setInt(int value);
		void setFloat(float value);
		void setBool(bool value);
		void setString(const std::string& value);
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
		Value& operator=(void*);
		Value& operator=(Type);

		bool operator==(const Value& right)const;

		Value() = default;
		Value(int val);
		Value(float val);
		Value(bool val);
		Value(const std::string& val);

	private:
		Type type = Undefined;
		bool isBoundPointer = false;

		union {
			int intVal, * intValP;
			float floatVal, * floatValP;
			bool boolVal, * boolValP;
			std::string* stringValP;
		} rep;
		std::string stringVal;
	};

}

typedef pitta::Value PittaValue;