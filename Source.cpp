#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"

int main() {

	std::string source = "1";
	pitta::Runtime runtime;

	pitta::Scanner scanner(source, &runtime);
	pitta::Parser parser(scanner.scanTokens(), &runtime);
	pitta::Interpreter interpreter(&runtime);
	interpreter.interpret(parser.parse<void, pitta::Value>());
	return 0;
}