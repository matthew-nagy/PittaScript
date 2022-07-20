#include "PittaValue.hpp"

#define basic_numerics \
switch (type) {\
		case Int:\
			if (isBoundValue())\
				return *rep.intValP;\
			else\
				return rep.intVal;\
		case Float:\
			if (isBoundValue())\
				return *rep.floatValP;\
			else\
				return rep.floatVal;\
		case Bool:\
			if (isBoundValue())\
				return *rep.boolValP;\
			else\
				return rep.boolVal;\
		default:\
			throw new PittaRuntimeException("No conversions available");\
		}

#define set_bound_values(value) \
switch (type) {\
	case Int:\
		*rep.intValP = value;\
		break;\
	case Float:\
		*rep.floatValP = value;\
		break;\
	case Bool:\
		*rep.boolValP = value;\
		break;\
	default:\
		throw new PittaRuntimeException("No conversions for bound values available");\
}

namespace pitta {

		//Int, Float, String, Bool, Null, Undefined, Instance, Class, Function
#define TS(type) { type, #type }
	const std::unordered_map<Type, std::string> c_typeToString = {
		TS(Int), TS(Float), TS(String), TS(Bool), TS(Null), TS(Undefined), TS(Instance), TS(Class), TS(Function)
	};
#undef TS

	std::string getSubstring(const std::string& from, int startIndex, int endIndex) {
		return from.substr(startIndex, endIndex - startIndex);
	}

	Type Value::getType()const {
		return type;
	}
	bool Value::isBoundValue()const {
		return isBoundPointer;
	}

	bool Value::isTruthy()const {
		switch (getType()) {
		case Int:
			return asInt() > 0;
		case Float:
			return asFloat() >= 0.0f;
		case Bool:
			return asBool();
		case String:
			return asString().size() > 0;
		case Null:
		case Undefined:
		default:
			return false;
		}
	}

	std::string Value::toString()const {
		switch (type) {
		case Int:
			if (isBoundValue())
				return std::to_string(*rep.intValP);
			else
				return std::to_string(rep.intVal);
		case Float:
			if (isBoundValue())
				return std::to_string(*rep.floatValP);
			else
				return std::to_string(rep.floatVal);
		case Bool:
			if (isBoundValue())
				return (*rep.boolValP) ? "true" : "false";
			else
				return rep.boolVal ? "true" : "false";
		case String:
			if (isBoundValue())
				return *rep.stringValP;
			else
				return stringVal;
		case Null:
			return "Null";
		case Undefined:
			return "Undefined";
		}
	}

	const int& Value::asInt()const {
		basic_numerics
	}
	const float& Value::asFloat()const {
		basic_numerics
	}
	const bool& Value::asBool()const {
		basic_numerics
	}
	const std::string& Value::asString()const {
		if (type == String) {
			if (isBoundValue())
				return *rep.stringValP;
			return stringVal;
		}
		throw new PittaRuntimeException("No string conversion acceptable");
	}

	void Value::setInt(int value) {
		if (isBoundValue()) {
			set_bound_values(value);
		}
		else {
			type = Int;
			rep.intVal = value;
		}
	}
	void Value::setFloat(float value) {
		if (isBoundValue()) {
			set_bound_values(value);
		}
		else {
			type = Float;
			rep.floatVal = value;
		}
	}
	void Value::setBool(bool value) {
		if (isBoundValue()) {
			set_bound_values(value);
		}
		else {
			type = Bool;
			rep.boolVal = value;
		}
	}
	void Value::setString(const std::string& value) {
		if (!(type == String || type == Null || type == Undefined))
			throw new PittaRuntimeException("Cannot set something as string if not string, null or undefined");
		if (isBoundValue()) {
			*rep.stringValP = value;
		}
		else {
			type = String;
			stringVal = value;
		}
	}
	void Value::setNull() {
		if (isBoundValue()) {
			throw new PittaRuntimeException("Cannot set a bound value to null");
		}
		type = Null;
	}
	void Value::setUndefined() {
		if (isBoundValue()) {
			throw new PittaRuntimeException("Cannot set a bound value to undefined");
		}
		type = Undefined;
	}

	void Value::bindInt(int* toBind) {
		type = Int;
		rep.intValP = toBind;
	}
	void Value::bindFloat(float* toBind) {
		type = Float;
		rep.floatValP = toBind;
	}
	void Value::bindBool(bool* toBind) {
		type = Bool;
		rep.boolValP = toBind;
	}
	void Value::bindString(std::string* toBind) {
		type = String;
		rep.stringValP = toBind;
	}

	Value& Value::operator=(const Value& right) {
		switch (right.type) {
		case Int:
			setInt(right.asInt());
			break;
		case Float:
			setFloat(right.asFloat());
			break;
		case Bool:
			setBool(right.asBool());
			break;
		case String:
			setString(right.asString());
			break;
		case Null:
			setNull();
			break;
		case Undefined:
			setUndefined();
			break;
		default:
			throw new PittaRuntimeException("Unknown type");
		}
		return *this;
	}
	Value& Value::operator=(int value) {
		setInt(value);
		return *this;
	}
	Value& Value::operator=(float value) {
		setFloat(value);
		return *this;
	}
	Value& Value::operator=(bool value) {
		setBool(value);
		return *this;
	}
	Value& Value::operator=(const std::string& value) {
		setString(value);
		return *this;
	}
	Value& Value::operator=(void*) {
		setNull();
		return *this;
	}
	Value& Value::operator=(Type) {
		setUndefined();
		return *this;
	}

	bool Value::operator==(const Value& right)const {
		try {
			switch (getType()) {
			case Null:
				return right.getType() == Null;
			case Undefined:
				return right.getType() == Undefined;
			case Int:
				return asInt() == right.asInt();
			case Float:
				return asFloat() == right.asFloat();
			case Bool:
				return asBool() == right.asBool();
			case String:
				return asString() == right.asString();
			default:
				printf("Comparison with unknown type\n");
			}
		}
		catch (...) {
			return false;
		}
		return false;
	}


}

#undef basic_numerics
#undef set_bound_values