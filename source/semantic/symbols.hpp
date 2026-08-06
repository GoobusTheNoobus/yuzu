#pragma once
#include "parser/parser.hpp"
#include <optional>
#include <unordered_map>

namespace yuzu {

    struct Symbol {
        std::string name; // Variable name
        BuiltinType type; // Resolved types
    };

    class SymbolTable {
      public:
        SymbolTable();

        void pushScope();
        void popScope();
        void define(const Symbol& symbol);

        std::optional<Symbol> lookup(const std::string& name);

      private:
        std::vector<std::unordered_map<std::string, Symbol>> scopes;
    };
} // namespace yuzu