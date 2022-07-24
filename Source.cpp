#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"
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
		pitta::Scanner scanner(buffer.str(), &runtime);
		auto tokens = scanner.scanTokens();
		for (pitta::Token& t : tokens)
			printf("%s\n", t.toString().c_str());
	}

	pitta::Scanner scanner(buffer.str(), &runtime);
	pitta::Parser<void, pitta::Value> parser(scanner.scanTokens(), &runtime);
	pitta::Interpreter interpreter(&runtime);
	interpreter.interpret(parser.parse().statements);

	std::string a;
	std::cout << "Ready to see the full power of cpp?";
	std::getline(std::cin, a);
	for (int i = 0; i < 20; i = i + 1) {
		printf("%d\n", fib(i));
	}

	return 0;
}