#include "PittaFunction.hpp"
#include <iostream>

namespace pitta{
    namespace stl{

        //typedef Value(*FunctionSigniture)(Interpreter*, const std::vector<Value>&);

        Value toInt(Interpreter*, const std::vector<Value>& values);
        Value toFloat(Interpreter*, const std::vector<Value>& values);
        Value toString(Interpreter*, const std::vector<Value>& values);
        Value toBool(Interpreter*, const std::vector<Value>& values);

        Value inputLine(Interpreter*, const std::vector<Value>& values);

        void addGlobalVariables(shared_data<Environment>& environment);

        const NativeCallable toIntCallable(1, toInt);
        const NativeCallable toFloatCallable(1, toFloat);
        const NativeCallable toStringCallable(1, toString);
        const NativeCallable toBoolCallable(1, toBool);

        const NativeCallable inputLineCallable(0, inputLine);

        shared_data<Environment> getEnvironment();
    }
}