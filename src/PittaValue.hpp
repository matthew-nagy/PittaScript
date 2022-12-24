#pragma once
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include "SharedData.hpp"
#include "PittaHigherTypes.hpp"

namespace pitta {

	enum Type : char {
		Int, Float, String, Bool, Null, Undefined, ClassInstance, ClassDef, Function
	};

	extern const std::unordered_map<Type, std::string> c_typeToString;

	class Callable;
	class Class;
	class Instance;

	std::string getSubstring(const std::string& from, int startIndex, int endIndex);

	class Value {
	public:
		Type getType()const;
		bool isBoundValue()const;

		std::string toString()const;

		bool isTruthy()const;

		int asInt()const;
		operator int()const;

		float asFloat()const;
		operator float()const;

		bool asBool()const;
		operator bool()const;

		std::string asString()const;
		operator std::string()const;

		const Callable* asCallable()const;
		operator const Callable* ()const;

		const Class* asClass()const;
		operator const Class* ()const;

		Instance* asInstance()const;
		operator Instance* ()const;

		//Shortcuts for when we just want the value
		vec2 asVec2()const;
		vec3 asVec3()const;
		vec4 asVec4()const;
		operator vec2()const;
		operator vec3()const;
		operator vec4()const;



		void setInt(int value);
		void setFloat(float value);
		void setBool(bool value);
		void setString(const std::string& value);
		void setCallable(const Callable* callable);
		void setClass(const Class* classDef);
		void setInstance(Instance* instance);
		/*void setVector(vec2 vec);
		void setVector(vec3 vec);
		void setVector(vec4 vec);*/
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
		Value& operator=(const Class* classDef);
		Value& operator=(Instance* instance);
		Value& operator=(Type);

		bool operator==(const Value& right)const;

		Value() = default;
		Value(Type t);
		Value(int val);
		Value(float val);
		Value(bool val);
		Value(const std::string & val);
		Value(int* val);
		Value(float* val);
		Value(bool* val);
		Value(std::string* val);
		Value(const Callable* callable);
		Value(const Class* classDef);
		Value(Instance* instance);
		Value(const Value& value);

	private:
		Type type = Undefined;
		bool isBoundPointer = false;

		union {
			int intVal, * intValP;
			float floatVal, * floatValP;
			bool boolVal, * boolValP;
			std::string* stringValP;
			const Callable* func;
			const Class* classDef;
			Instance* instance;
		} rep;
		std::unique_ptr<std::string> stringVal = std::make_unique<std::string>();
	};

	class PittaRuntimeException final : public std::runtime_error {
	public:
		std::string details;

		const char* what()const noexcept override;

		PittaRuntimeException(const std::string& detail);
	};

	class PittaReturn final : public std::runtime_error {
	public:
		Value value;

		PittaReturn(Value value);
	};

}

typedef pitta::Value PittaValue;