#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"
#include "PittaResolver.hpp"
#include "PittaStl.hpp"
#include <fstream>
#include <sstream>

int fib(int n) {
	if (n <= 1) return n;
	return fib(n - 2) + fib(n - 1);
}

#include <iostream>
#include "PittaIntegration.hpp"

int main() {

	std::stringstream buffer;

	std::string programs[] = { "ProgramFib.txt", "ProgramCollision.txt", "ProgramIntegrate.txt", "ProgramPlayground.txt"};
	const int numOfPrograms = 4;
	printf("What program do you want to run?:\n");
	for (int i = 0; i < numOfPrograms; i++)
		printf("\t%s\t%d\n", programs[i].c_str(), i);
	printf(">>");
	std::string choice = "";
	std::getline(std::cin, choice);
	int choiceNum = std::atoi(choice.c_str());
	if (choiceNum < 0 || choiceNum >= numOfPrograms) {
		printf("That was not a valid choice \n\n\n");
		return main();
	}

	std::ifstream file(programs[choiceNum]);
	buffer << file.rdbuf();

	pitta::Runtime runtime;
	{
		/*pitta::Parser<std::string, std::string> printParse(pitta::Scanner(buffer.str(), &runtime).scanTokens(), &runtime);
		auto stringParse = printParse.parse();
		pitta::StmtASTPrinter astP;
		printf("\n\n\n");
		for (auto& statement : stringParse.statements)
			printf("%s\n", statement->accept(&astP).c_str());
		printf("\n\n\n");*/
	}

	pitta::Scanner scanner(buffer.str(), &runtime);
	pitta::Parser<void, pitta::Value> parser(scanner.scanTokens(), &runtime);
	auto tree = parser.parse();

	std::shared_ptr<pitta::Environment> globals = pitta::stl::getEnvironment();
	globals->define("IT", pitta::ITClass);
	pitta::IT test("Harold", 21, "Not a hand murderer");
	globals->define("harold", pitta::ITClass->bindExistingInstance(&test));

	pitta::Interpreter interpreter(&runtime, globals);
	pitta::Resolver r(&interpreter);
	r.sweepStatements(tree.statements);

	if (!runtime.hadError) {
		interpreter.interpret(tree.statements);
	}
	else {
		printf("There was an error, cannot run!\n");
	}

	printf("And now in c++\n");
	test.fullIntro();

	system("pause");

	return 0;
}