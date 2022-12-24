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

<<<<<<< HEAD
        void addGlobalVariables(std::shared_ptr<Environment>& environment);
=======
        void addGlobalVariables(shared_data<Environment>& environment){
            environment->assign("Null", Null);
            environment->assign("Undefined", Undefined);
        }
>>>>>>> e79dc48610978f94e3bff458055f44eba0c12334

        const NativeCallable toIntCallable(1, toInt);
        const NativeCallable toFloatCallable(1, toFloat);
        const NativeCallable toStringCallable(1, toString);
        const NativeCallable toBoolCallable(1, toBool);

        const NativeCallable inputLineCallable(0, inputLine);

<<<<<<< HEAD
        std::shared_ptr<Environment> getEnvironment();
=======
        shared_data<Environment> getEnvironment(){
            shared_data<Environment> environment = make_shared_data<Environment>();
            addGlobalVariables(environment);

            environment->define("int", &toIntCallable);
            environment->define("float", &toFloatCallable);
            environment->define("string", &toStringCallable);
            environment->define("str", &toStringCallable);
            environment->define("bool", &toBoolCallable);

            environment->define("input", &inputLineCallable);

            return environment;
        }
>>>>>>> e79dc48610978f94e3bff458055f44eba0c12334
    }
}