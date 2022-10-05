#include "PittaFunction.hpp"
#include <iostream>

namespace pitta{
    namespace stl{

        //typedef Value(*FunctionSigniture)(Interpreter*, const std::vector<Value>&);

        Value toInt(Interpreter*, const std::vector<Value>& values){
            const Value& val = values[0];
            if(val.getType() == String){
                return std::stoi(val.asString());
            }
            return val.asInt();
        }
        Value toFloat(Interpreter*, const std::vector<Value>& values){
            const Value& val = values[0];
            if(val.getType() == String){
                return std::stof(val.asString());
            }
            return val.asInt();
        }
        Value toString(Interpreter*, const std::vector<Value>& values){
            const Value& val = values[0];
            if(val.getType() == String){
                return val.asString();
            }
            return val.toString();
        }
        Value toBool(Interpreter*, const std::vector<Value>& values){
            const Value& val = values[0];
            return val.isTruthy();
        }

        Value inputLine(Interpreter*, const std::vector<Value>& values){
            std::string line;
            std::getline(std::cin, line);
            return line;
        }

        void addGlobalVariables(std::shared_ptr<Environment>& environment){
            environment->assign("Null", Null);
            environment->assign("Undefined", Undefined);
        }

        const NativeCallable toIntCallable(1, toInt);
        const NativeCallable toFloatCallable(1, toFloat);
        const NativeCallable toStringCallable(1, toString);
        const NativeCallable toBoolCallable(1, toBool);

        const NativeCallable inputLineCallable(0, inputLine);

        std::shared_ptr<Environment> getEnvironment(){
            std::shared_ptr<Environment> environment = std::make_shared<Environment>();
            addGlobalVariables(environment);

            environment->define("int", &toIntCallable);
            environment->define("float", &toFloatCallable);
            environment->define("string", &toStringCallable);
            environment->define("bool", &toBoolCallable);

            environment->define("input", &inputLineCallable);

            return environment;
        }
    }
}