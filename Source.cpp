#include "PittaParser.hpp"
#include "PittaRuntime.hpp"
#include "PittaInterpreter.hpp"

class A {

};

class B : public A {

};

int main() {

	std::string source = "var a = \"global a\";\
							var b = \"global b\";\
							var c = \"global c\";\
							{\
								var a = \"outer a\";\
								var b = \"outer b\";\
								{\
									var a = \"inner a\";\
									print a;\
									print b;\
									print c;\
								}\
								print a;\
								print b;\
								print c;\
							}\
							print a;\
							print b;\
							print c;\
							if(false){print 5;}else if(false){print 3;}else print 1;";


	pitta::Runtime runtime;

	pitta::Scanner scanner(source, &runtime);
	pitta::Parser parser(scanner.scanTokens(), &runtime);
	pitta::Interpreter interpreter(&runtime);
	interpreter.interpret(parser.parse<void, pitta::Value>());
	return 0;
}