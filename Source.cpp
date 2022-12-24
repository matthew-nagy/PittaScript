#include "src/PittaParser.hpp"
#include "src/PittaRuntime.hpp"
#include "src/PittaInterpreter.hpp"
#include "src/PittaResolver.hpp"
#include "src/PittaStl.hpp"
#include <fstream>
#include <sstream>
#include <chrono>

int fib(int n) {
	if (n <= 1) return n;
	return fib(n - 2) + fib(n - 1);
}

#include <iostream>
#define PITTA_INTEGRATION_TEST
#include "src/PittaIntegration.hpp"
#include "src/SharedData.hpp"


int main() {
	std::stringstream buffer;

	std::string programs[] = { "ProgramFib.txt", "ProgramCollision.txt", "ProgramIntegrate.txt", "ProgramPlayground.txt", "example_config"};
	const int numOfPrograms = 5;
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

	std::chrono::steady_clock::time_point begin_compile = std::chrono::steady_clock::now();

	pitta::Scanner scanner(buffer.str(), &runtime);
	pitta::Parser<void, pitta::Value> parser(scanner.scanTokens(), &runtime);
	auto tree = parser.parse();
	std::chrono::steady_clock::time_point end_compile = std::chrono::steady_clock::now();
	std::cout << "Compile time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_compile - begin_compile).count() << "[ms]" << std::endl;

	pitta::shared_data<pitta::Environment> globals = pitta::stl::getEnvironment();
	globals->define("IT", pitta::ITClass);
	pitta::IT test("Harold", 21, "Not a hand murderer");
	globals->define("harold", pitta::ITClass->bindExistingInstance(&test));

	auto preRunningValues = globals->getDefinedValueNames();

	pitta::Interpreter interpreter(&runtime, globals);
	pitta::Resolver r(&interpreter);
	std::chrono::steady_clock::time_point begin_resolve = std::chrono::steady_clock::now();
	r.sweepStatements(tree.statements);
	std::chrono::steady_clock::time_point end_resolve = std::chrono::steady_clock::now();
	std::cout << "Resolve time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_resolve - begin_resolve).count() << "[ms]" << std::endl;

	if (!runtime.hadError) {
		std::chrono::steady_clock::time_point begin_run = std::chrono::steady_clock::now();
		interpreter.interpret(tree.statements);
		std::chrono::steady_clock::time_point end_run = std::chrono::steady_clock::now();
		std::cout << "Run time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_run - begin_run).count() << "[ms]" << std::endl;
	}
	else {
		printf("There was an error, cannot run!\n");
	}

	printf("And now in c++\n");
	test.fullIntro();

	system("pause");

	return 0;
}