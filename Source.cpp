#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"
#include "PittaResolver.hpp"
#include <fstream>
#include <sstream>

int fib(int n) {
	if (n <= 1) return n;
	return fib(n - 2) + fib(n - 1);
}

#include <iostream>

int main() {

	std::stringstream buffer;
	std::ifstream file("Program.txt");
	buffer << file.rdbuf();

	pitta::Runtime runtime;
	{
		pitta::Parser<std::string, std::string> printParse(pitta::Scanner(buffer.str(), &runtime).scanTokens(), &runtime);
		auto stringParse = printParse.parse();
		pitta::StmtASTPrinter astP;
		printf("\n\n\n");
		for (auto& statement : stringParse.statements)
			printf("%s\n", statement->accept(&astP).c_str());
		printf("\n\n\n");
	}

	pitta::Scanner scanner(buffer.str(), &runtime);
	pitta::Parser<void, pitta::Value> parser(scanner.scanTokens(), &runtime);
	auto tree = parser.parse();
	pitta::Interpreter interpreter(&runtime);
	pitta::Resolver r(&interpreter);
	r.sweepStatements(tree.statements);
	interpreter.interpret(tree.statements);

	std::string a;
	std::cout << "Ready to see the full power of cpp?";
	std::getline(std::cin, a);
	for (int i = 0; i < 20; i = i + 1) {
		printf("%d\n", fib(i));
	}

	return 0;
}