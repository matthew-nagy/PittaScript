#pragma once

#ifndef ADDITIONAL_VEC2_FUNCS
#define ADDITIONAL_VEC2_FUNCS
#define ADDITIONAL_VEC2_FUNC_DEFS
#endif

#ifndef ADDITIONAL_VEC3_FUNCS
#define ADDITIONAL_VEC3_FUNCS
#define ADDITIONAL_VEC3_FUNC_DEFS
#endif

#ifndef ADDITIONAL_VEC4_FUNCS
#define ADDITIONAL_VEC4_FUNCS
#define ADDITIONAL_VEC4_FUNC_DEFS
#endif

namespace pitta {
	template<class T>
	class IntegratedClass;

	class Environment;

	class vec2 {
	public:
		float x, y;

		vec2 operator+(const vec2& right)const;
		vec2 operator-(const vec2& right)const;
		vec2 operator*(float right)const;
		vec2 operator/(float right)const;

		ADDITIONAL_VEC2_FUNCS

		vec2() = default;
		vec2(float x, float y);
	};
	extern IntegratedClass<vec2>* vec2Binding;

	class vec3 {
	public:
		float x, y, z;

		vec3 operator+(const vec3& right)const;
		vec3 operator-(const vec3& right)const;
		vec3 operator*(float right)const;
		vec3 operator/(float right)const;

		ADDITIONAL_VEC3_FUNCS

		vec3() = default;
		vec3(float x, float y, float z);
	};
	extern IntegratedClass<vec3>* vec3Binding;

	class vec4 {
	public:
		float x, y, z, w;

		vec4 operator+(const vec4& right)const;
		vec4 operator-(const vec4& right)const;
		vec4 operator*(float right)const;
		vec4 operator/(float right)const;

		ADDITIONAL_VEC4_FUNCS

		vec4() = default;
		vec4(float x, float y, float z, float w);
	};
	extern IntegratedClass<vec4>* vec4Binding;

	void addTypeBindings(Environment* environment);
}