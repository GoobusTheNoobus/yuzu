#include "semantic/functions.hpp"

namespace yuzu {

    void FunctionTable::define(const Function& func) {
        map[func.name] = func;
    }

    std::optional<Function> FunctionTable::lookup(const std::string& name) {
        auto found = map.find(name);

        if (found != map.end()) {
            return found->second;
        }

        return std::nullopt;
    }
} // namespace yuzu