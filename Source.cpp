#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"

int main() {

	std::string source = "(true << 4)";
	pitta::Runtime runtime;
	{
		pitta::Scanner scanner(source, &runtime);
		auto tokens = scanner.scanTokens();
		for (pitta::Token& t : tokens)
			printf("> %s\n", t.toString().c_str());
	}
	printf("--\n");
	{
		pitta::Scanner scanner(source, &runtime);
		pitta::Parser parser(scanner.scanTokens(), &runtime);
		pitta::ASTPrinter printer;
		printf("%s\n", printer.print(parser.parse<std::string>()).c_str());
	}
	printf("--\n");
	{
		pitta::Scanner scanner(source, &runtime);
		pitta::Parser parser(scanner.scanTokens(), &runtime);
		pitta::Interpreter interpreter(&runtime);
		interpreter.interpret(parser.parse<pitta::Value>());
	}
	return 0;
}