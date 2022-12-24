#include "PittaHigherTypes.hpp"
#include "PittaIntegration.hpp"
#include "PittaInterpreter.hpp"

namespace pitta {
	vec2 vec2::operator+(const vec2& right)const {
		return vec2(x + right.x, y + right.y);
	}
	vec2 vec2::operator-(const vec2& right)const {
		return vec2(x - right.x, y - right.y);
	}
	vec2 vec2::operator*(float right)const {
		return vec2(x * right, y * right);
	}
	vec2 vec2::operator/(float right)const {
		return vec2(x / right, y / right);
	}
	vec2::vec2(float x, float y):
		x(x),
		y(y)
	{}


	vec3 vec3::operator+(const vec3& right)const {
		return vec3(x + right.x, y + right.y, z + right.z);
	}
	vec3 vec3::operator-(const vec3& right)const {
		return vec3(x - right.x, y - right.y, z - right.z);
	}
	vec3 vec3::operator*(float right)const {
		return vec3(x * right, y * right, z * right);
	}
	vec3 vec3::operator/(float right)const {
		return vec3(x / right, y / right, z / right);
	}
	vec3::vec3(float x, float y, float z):
		x(x),
		y(y),
		z(z)
	{}


	vec4 vec4::operator+(const vec4& right)const {
		return vec4(x + right.x, y + right.y, z + right.z, w + right.w);
	}
	vec4 vec4::operator-(const vec4& right)const {
		return vec4(x - right.x, y - right.y, z - right.z, w - right.w);
	}
	vec4 vec4::operator*(float right)const {
		return vec4(x * right, y * right, z * right, w * right);
	}
	vec4 vec4::operator/(float right)const {
		return vec4(x / right, y / right, z / right, w / right);
	}
	vec4::vec4(float x, float y, float z, float w):
		x(x),
		y(y),
		z(z),
		w(w)
	{}



	vec2* generateNewVec2(pitta::Interpreter*, const std::vector<pitta::Value>& arguments) {
		return new vec2(arguments[0].asFloat(), arguments[1].asFloat());
	}

	std::unordered_map<std::string, pitta::Value> getVec2Fields(vec2* vec) {
		std::unordered_map<std::string, pitta::Value> binding;

		binding.emplace("x", &vec->x);
		binding.emplace("y", &vec->y);

		return binding;
	}

	IntegratedClass<vec2>* vec2Binding = new pitta::IntegratedClass<vec2>(
		"vec2", std::unordered_map<std::string, pitta::Callable*>(), 2, generateNewVec2, getVec2Fields
		);



	vec3* generateNewVec3(pitta::Interpreter*, const std::vector<pitta::Value>& arguments) {
		return new vec3(arguments[0].asFloat(), arguments[1].asFloat(), arguments[2].asFloat());
	}

	std::unordered_map<std::string, pitta::Value> getVec3Fields(vec3* vec) {
		std::unordered_map<std::string, pitta::Value> binding;

		binding.emplace("x", &vec->x);
		binding.emplace("y", &vec->y);
		binding.emplace("z", &vec->z);

		return binding;
	}

	IntegratedClass<vec3>* vec3Binding = new pitta::IntegratedClass<vec3>(
		"vec3", std::unordered_map<std::string, pitta::Callable*>(), 3, generateNewVec3, getVec3Fields
		);


	vec4* generateNewVec4(pitta::Interpreter*, const std::vector<pitta::Value>& arguments) {
		return new vec4(arguments[0].asFloat(), arguments[1].asFloat(), arguments[2].asFloat(), arguments[3].asFloat());
	}

	std::unordered_map<std::string, pitta::Value> getVec4Fields(vec4* vec) {
		std::unordered_map<std::string, pitta::Value> binding;

		binding.emplace("x", &vec->x);
		binding.emplace("y", &vec->y);
		binding.emplace("z", &vec->z);
		binding.emplace("w", &vec->w);

		return binding;
	}

	IntegratedClass<vec4>* vec4Binding = new pitta::IntegratedClass<vec4>(
		"vec4", std::unordered_map<std::string, pitta::Callable*>(), 4, generateNewVec4, getVec4Fields
		);




	void addTypeBindings(shared_data<pitta::Environment>& environment) {
		addTypeBindings(environment.get());
	}
	void addTypeBindings(pitta::Environment* environment) {
		environment->define(vec2Binding->getName(), vec2Binding);
		environment->define(vec3Binding->getName(), vec3Binding);
		environment->define(vec4Binding->getName(), vec4Binding);
	}






	ADDITIONAL_VEC2_FUNC_DEFS;
	ADDITIONAL_VEC3_FUNC_DEFS;
	ADDITIONAL_VEC4_FUNC_DEFS;
}