#pragma once
#include "parser/parser.hpp"
#include <optional>
#include <unordered_map>

namespace yuzu {

    struct Function {
        std::string name;                 // Function name
        BuiltinType type;                 // Return types
        std::vector<BuiltinType> argList; // Overloads
    };

    class FunctionTable {
      public:
        FunctionTable() = default;
        void define(const Function& func);

        std::optional<Function> lookup(const std::string& name);

      private:
        std::unordered_map<std::string, Function> map;
    };
} // namespace yuzu