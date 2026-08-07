#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include "parser/node.hpp"

namespace yuzu {

    class CodeGen {
      public:
        CodeGen();

        llvm::Module* generate(Program& program);

      private:
        llvm::LLVMContext context;
        llvm::IRBuilder<> builder;
        std::unique_ptr<llvm::Module> module;
        std::unordered_map<std::string, llvm::Value*> variables;

        void generateStatement(Node& node);
        llvm::Value* generateExpression(Node& node);

        llvm::Type* llvmType(BuiltinType type);
    };

} // namespace yuzu