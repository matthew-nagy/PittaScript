#pragma once

#include <string>
#include <stdio.h>

#include "PittaTokenScanner.hpp"

namespace pitta {

	class Runtime {
	public:
		bool hadError = false;

		void error(int line, const std::string& message) {
			report(line, "", message);
		}

		void error(Token token, const std::string& message) {
			if (token.type == TokenType::END_OF_FILE) {
				report(token.line, " at end", message);
			}
			else {
				report(token.line, " at '" + token.lexeme + "'", message);
			}
		}

		std::vector<Token> scan(const std::string& source) {
			Scanner scanner(source, this);
			return scanner.scanTokens();
		}

	private:
		void report(int line, const std::string& where, const std::string& message) {
			printf("[line %d] Error%s: %s\n", line, where.c_str(), message.c_str());
			hadError = true;
		}
	};

}

typedef pitta::Runtime PittaRuntime;