#include "PittaValue.hpp"
#include "PittaFunction.hpp"
#include "PittaClass.hpp"
#include "PittaIntegration.hpp"

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



	const char* PittaRuntimeException::what()const noexcept {
		return (std::string("Pitta runtime exception: ") + std::runtime_error::what()).c_str();
	}

	PittaRuntimeException::PittaRuntimeException(const std::string& detail) :
		std::runtime_error(detail),
		details(detail)
	{}

	PittaReturn::PittaReturn(Value value) :
		std::runtime_error("Return exception; should be caught"),
		value(value)
	{}



		//Int, Float, String, Bool, Null, Undefined, ClassInstance, ClassDef, Function
#define TS(type) { type, #type }
	const std::unordered_map<Type, std::string> c_typeToString = {
		TS(Int), TS(Float), TS(String), TS(Bool), TS(Null), TS(Undefined), TS(ClassInstance), TS(ClassDef), TS(Function)
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
		case Function:
			return true;
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
				return *stringVal;
		case Function:
			return "Function " + rep.func->getName() + " with arity " + std::to_string(rep.func->getArity());
		case ClassDef:
			return rep.classDef->asString();
		case ClassInstance:
			return rep.instance->asString();
		case Null:
			return "Null";
		case Undefined:
			return "Undefined";
		default:
			throw new PittaRuntimeException("Error: Unknown type");
			return "Error: Unknown type";
		}
	}

	int Value::asInt()const {
		basic_numerics
	}
	Value::operator int()const {
		return asInt();
	}
	float Value::asFloat()const {
		basic_numerics
	}
	Value::operator float()const {
		return asFloat();
	}
	bool Value::asBool()const {
		basic_numerics
	}
	Value::operator bool()const {
		return asBool();
	}
	const std::string& Value::asString()const {
		if (type == String) {
			if (isBoundValue())
				return *rep.stringValP;
			return *stringVal;
		}
		throw new PittaRuntimeException("No string conversion acceptable");
	}
	Value::operator const std::string& ()const {
		return asString();
	}
	const Callable* Value::asCallable()const {
		if (type == Function)
			return rep.func;
		else if (type == ClassDef)
			return rep.classDef;
		throw new PittaRuntimeException("No function conversion acceptable");
	}
	Value::operator const pitta::Callable* ()const {
		return asCallable();
	}

	const Class* Value::asClass()const {
		return rep.classDef;
	}
	Value::operator const pitta::Class* ()const {
		return asClass();
	}

	Instance* Value::asInstance()const {
		return rep.instance;
	}
	Value::operator pitta::Instance* ()const {
		return asInstance();
	}

	vec2 Value::asVec2()const {
		if ((void*)rep.instance->getDefinition() != (void*)vec2Binding) {
			throw new PittaRuntimeException("Value is not an instance of vec2");
		}
		return *vec2Binding->unpack(*this);
	}
	vec3 Value::asVec3()const {
		if ((void*)rep.instance->getDefinition() != (void*)vec3Binding) {
			throw new PittaRuntimeException("Value is not an instance of vec3");
		}
		return *vec3Binding->unpack(*this);
	}
	vec4 Value::asVec4()const {
		if ((void*)rep.instance->getDefinition() != (void*)vec4Binding) {
			throw new PittaRuntimeException("Value is not an instance of vec4");
		}
		return *vec4Binding->unpack(*this);
	}
	Value::operator pitta::vec2()const { return asVec2(); }
	Value::operator pitta::vec3()const { return asVec3(); }
	Value::operator pitta::vec4()const { return asVec4(); }

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
			*stringVal = value;
		}
	}
	void Value::setCallable(const Callable* callable) {
		if (!(type == Null || type == Undefined || type == Function))
			throw new PittaRuntimeException("Cannot assign a non-function, null or undefined value to a function");
		type = Function;
		rep.func = callable;
	}
	void Value::setClass(const Class* classDef) {
		if (!(type == Null || type == Undefined))
			throw new PittaRuntimeException("Cannot assign a class to a non null or undefined value");
		type = ClassDef;
		rep.classDef = classDef;
	}
	void Value::setInstance(Instance* instance) {
		if (!(type == Null || type == Undefined))
			throw new PittaRuntimeException("Cannot assign an instance of a class to a non null or undefined value");
		type = ClassInstance;
		rep.instance = instance;
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
		isBoundPointer = true;
		type = Int;
		rep.intValP = toBind;
	}
	void Value::bindFloat(float* toBind) {
		isBoundPointer = true;
		type = Float;
		rep.floatValP = toBind;
	}
	void Value::bindBool(bool* toBind) {
		isBoundPointer = true;
		type = Bool;
		rep.boolValP = toBind;
	}
	void Value::bindString(std::string* toBind) {
		isBoundPointer = true;
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
		case Function:
			setCallable(right.asCallable());
			break;
		case ClassDef:
			setClass(right.asClass());
			break;
		case ClassInstance:
			setInstance(right.asInstance());
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
	Value& Value::operator=(const Callable* callable) {
		if (callable == nullptr)
			setNull();
		else
			setCallable(callable);
		return *this;
	}	
	Value& Value::operator=(const Class* classDef) {
		setClass(classDef);
		return *this;
	}
	Value& Value::operator=(Instance* instance) {
		setInstance(instance);
		return *this;
	}
	Value& Value::operator=(Type type) {
		if (type == Undefined)
			setUndefined();
		else if (type == Null)
			setNull();
		else
			throw(0);
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

	Value::Value(Type t) {
		if (t == Null || t == Undefined) {
			type = t;
		}
		else {
			throw - 1;
		}
	}
	Value::Value(int val) {
		setInt(val);
	}
	Value::Value(float val) {
		setFloat(val);
	}
	Value::Value(bool val) {
		setBool(val);
	}
	Value::Value(const std::string& val) {
		setString(val);
	}
	Value::Value(int* val) {
		bindInt(val);
	}
	Value::Value(float* val) {
		bindFloat(val);
	}
	Value::Value(bool* val) {
		bindBool(val);
	}
	Value::Value(std::string* val) {
		bindString(val);
	}
	Value::Value(const Callable* callable) {
		setCallable(callable);
	}
	Value::Value(const Class* classDef) {
		setClass(classDef);
	}
	Value::Value(Instance* instance) {
		setInstance(instance);
	}
	Value::Value(const Value& value) {
		rep = value.rep;
		type = value.type;
		isBoundPointer = value.isBoundPointer;
		if (type == String && !isBoundPointer)
			*stringVal = *value.stringVal;
	}

}

#undef basic_numerics
#undef set_bound_values