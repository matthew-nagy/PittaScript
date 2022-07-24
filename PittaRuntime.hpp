#pragma once

#include <string>
#include <stdio.h>

#include "PittaTokenScanner.hpp"

namespace pitta {

	class Runtime {
	public:
		bool hadError = false;

		void error(int line, const std::string& message);

		void error(Token token, const std::string& message);

		void runtimeError(PittaRuntimeException* exception);

		std::vector<Token> scan(const std::string& source);

	private:
		void report(int line, const std::string& where, const std::string& message);

		void report(PittaRuntimeException* exception);
	};

}

typedef pitta::Runtime PittaRuntime;