#pragma once
#include "core/core.hpp"
#include <llvm/IR/Module.h>

namespace yuzu {
    std::string readFile(const char path[]);
    void writeIRFile(const char name[], const llvm::Module* module);
    void writeObjectFile(const char name[], llvm::Module* _module);
    void link(const char* objectFile, const char* outputExecutable);
} // namespace yuzu