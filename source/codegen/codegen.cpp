#include "codegen/codegen.hpp"
#include "diagnostic/error.hpp"

namespace yuzu {
    CodeGen::CodeGen() : builder(context) {
        module = std::make_unique<llvm::Module>("module", context);
    }

    llvm::Module* CodeGen::generate(Program& program) {
        // Entry point I am guessing?
        auto mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
        auto main = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module.get());

        // Is this the definition of main()?
        auto entry = llvm::BasicBlock::Create(context, "entry", main);

        builder.SetInsertPoint(entry);

        for (auto& child : program.children) {
            generateStatement(*child);
        }

        // Fallback return (in case program doesn't have one)
        if (!builder.GetInsertBlock()->getTerminator()) {
            builder.CreateRet(builder.getInt32(0));
        }

        return module.get();
    }

    void CodeGen::generateStatement(Node& node) {
        switch (node.type) {
        case NodeType::VariableDeclaration: {
            auto& varDecl = static_cast<VarDecleration&>(node);
            llvm::Value* value = generateExpression(*varDecl.value);

            auto alloc = builder.CreateAlloca(llvmType(varDecl.resolvedType), nullptr, varDecl.name);

            builder.CreateStore(value, alloc);

            variables[varDecl.name] = alloc;
            break;
        }
        case NodeType::Exit: {
            auto& exit = static_cast<Exit&>(node);
            auto value = generateExpression(*exit.code);

            builder.CreateRet(value);
            break;
        }
        default: {
            diagnostic::throwError(diagnostic::ErrorType::InternalError,
                                   "Unfortunately, the only supported statements for codegen are exit and variable declaration. Sorry!");
        }
        }
    }

    llvm::Value* CodeGen::generateExpression(Node& node) {
        switch (node.type) {
        case NodeType::IntegerLiteral: {
            auto& integer = static_cast<IntegerLiteral&>(node);

            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), integer.data);
        }
        case NodeType::BinaryExpression: {
            auto& binary = static_cast<BinaryNode&>(node);

            auto left = generateExpression(*binary.left);
            auto right = generateExpression(*binary.right);

            switch (binary.op) {
            case BinaryOperator::Plus:
                return builder.CreateAdd(left, right);
            case BinaryOperator::Minus:
                return builder.CreateSub(left, right);
            case BinaryOperator::Star:
                return builder.CreateMul(left, right);
            case BinaryOperator::Slash:
                return builder.CreateSDiv(left, right);
            case BinaryOperator::Percent:
                return builder.CreateSRem(left, right);
            default:
                diagnostic::throwError(diagnostic::ErrorType::IntegerError, "Unknown binary operation");
            }

            break;
        }
        case NodeType::Identifier: {
            auto& identifier = static_cast<Identifier&>(node);

            auto it = variables.find(identifier.name);
            if (it == variables.end())
                diagnostic::throwError(diagnostic::ErrorType::InternalError,
                                       "Undeclared identifier found in CodeGen. Did you forget to perform semantic analysis?");

            llvm::Value* alloc = it->second;

            return builder.CreateLoad(llvmType(identifier.resolvedType), alloc, identifier.name);
        }
        case NodeType::Cast: {
            auto& cast = static_cast<CastNode&>(node);

            llvm::Value* value = generateExpression(*cast.thing);

            llvm::Type* fromType = value->getType();
            llvm::Type* toType = llvmType(cast.to);

            if (fromType == toType)
                return value;

            if (fromType->isIntegerTy() && toType->isIntegerTy())
                return builder.CreateIntCast(value, toType, true, "cast");

            if (fromType->isIntegerTy() && toType->isFloatingPointTy()) {
                return builder.CreateSIToFP(value, toType, "cast");
            }

            if (fromType->isFloatingPointTy() && toType->isIntegerTy()) {
                return builder.CreateFPToSI(value, toType, "cast");
            }

            if (fromType->isFloatingPointTy() && toType->isFloatingPointTy()) {
                return builder.CreateFPCast(value, toType, "cast");
            }

            diagnostic::throwError(diagnostic::ErrorType::InternalError, "Unsupported cast in codegen");
        }
        default: {
            std::cout << (int)node.type << std::endl;
            diagnostic::throwError(
                diagnostic::ErrorType::InternalError,
                "Unfortunately, the only supported expressions for codegen are int literals and binary expressions. Sorry!");
        }
        }

        return nullptr;
    }

    llvm::Type* CodeGen::llvmType(BuiltinType type) {
        switch (type) {
        case BuiltinType::I8:
            return builder.getInt8Ty();
        case BuiltinType::I16:
            return builder.getInt16Ty();
        case BuiltinType::I32:
            return builder.getInt32Ty();
        case BuiltinType::I64:
            return builder.getInt64Ty();
        case BuiltinType::F32:
            return builder.getFloatTy();
        case BuiltinType::F64:
            return builder.getDoubleTy();
        case BuiltinType::Char:
            return builder.getInt8Ty();
        case BuiltinType::Bool:
            return builder.getInt1Ty();
        default:

            diagnostic::throwError(diagnostic::ErrorType::TypeError, "Unknown type");
            return nullptr;
        }
    }
} // namespace yuzu