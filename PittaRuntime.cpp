#include "PittaRuntime.hpp"

namespace pitta {

	void Runtime::error(int line, const std::string& message) {
		report(line, "", message);
	}

	void Runtime::error(Token token, const std::string& message) {
		if (token.type == TokenType::END_OF_FILE) {
			report(token.line, " at end", message);
		}
		else {
			report(token.line, " at '" + token.lexeme + "'", message);
		}
	}

	void Runtime::runtimeError(PittaRuntimeException* exception) {
		report(exception);
	}

	std::vector<Token> Runtime::scan(const std::string& source) {
		Scanner scanner(source, this);
		return scanner.scanTokens();
	}


	void Runtime::report(int line, const std::string& where, const std::string& message) {
		printf("[line %d] Error%s: %s\n", line, where.c_str(), message.c_str());
		hadError = true;
	}

	void Runtime::report(PittaRuntimeException* exception) {
		printf("Runtime error: %s\n", exception->details.c_str());
		hadError = true;
	}

}