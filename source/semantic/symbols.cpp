#include "semantic/symbols.hpp"
#include "diagnostic/error.hpp"

namespace yuzu {

    SymbolTable::SymbolTable() {
        pushScope(); // push global
    }

    void SymbolTable::pushScope() {
        scopes.push_back({});
    }

    void SymbolTable::popScope() {
        scopes.pop_back();
    }

    void SymbolTable::define(const Symbol& symbol) {
        auto& currentScope = scopes.back();

        if (currentScope.find(symbol.name) != currentScope.end()) {
            diagnostic::throwError(diagnostic::ErrorType::NameError, "Variable '" + symbol.name + "' already defined");
        }

        currentScope[symbol.name] = symbol;
    }

    auto SymbolTable::lookup(const std::string& name) -> std::optional<Symbol> {
        // iterate scopes

        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);

            if (found != it->end())
                return found->second;
        }

        return std::nullopt;
    }
} // namespace yuzu