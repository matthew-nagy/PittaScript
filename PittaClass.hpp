#pragma once
#include <vector>
#include "PittaValue.hpp"

namespace pitta {

	class Class {
	public:
		const std::string name;

		std::string asString()const {
			return name;
		}

		Class(const std::string& name) :
			name(name)
		{}
	};

}