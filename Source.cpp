#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"
#include <fstream>
#include <sstream>

int main() {

	std::stringstream buffer;
	std::ifstream file("Program.txt");
	buffer << file.rdbuf();

	pitta::Runtime runtime;

	pitta::Scanner scanner(buffer.str(), &runtime);
	pitta::Parser<void, pitta::Value> parser(scanner.scanTokens(), &runtime);
	pitta::Interpreter interpreter(&runtime);
	interpreter.interpret(parser.parse().statements);
	return 0;
}