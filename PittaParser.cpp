#include "PittaParser.hpp"
#include "PittaRuntime.hpp"

namespace pitta {
	PittaRuntimeException* Parser::error(Token token, const std::string& message) {
		runtime->error(token, message);
		return new PittaRuntimeException("See log for detail");
	}
}