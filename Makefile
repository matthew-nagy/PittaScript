default:
	clang++ -std=c++17 -Wall PittaClass.cpp PittaEnvironment.cpp PittaInterpreter.cpp PittaRuntime.cpp PittaTokenScanner.cpp PittaValue.cpp Source.cpp -o Main
