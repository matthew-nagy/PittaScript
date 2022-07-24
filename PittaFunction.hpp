#pragma once

#include "PittaValue.hpp"
#include "PittaInterpreter.hpp"


namespace pitta {

	class Function {
	public:

		Value operator()(Interpreter* interpreter, const std::vector<Value>& arguments);

	};

}