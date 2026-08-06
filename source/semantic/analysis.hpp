#pragma once
#include "semantic/functions.hpp"
#include "semantic/symbols.hpp"

namespace yuzu {
    struct ExpressionResult {
        BuiltinType resolvedType;
        bool isConstantInt;
        i64 constantInt;
    };

    class Analysis {
      public:
        Analysis() = default;
        void analyze(Program& program);

      private:
        void analyzeStatement(Node& node);
        void analyzeDeclaration(VarDecleration& varDecl);
        void analyzeAssignment(VarAssignment& varAss);
        void analyzeCall(FunctionCallNode& callNode);
        ExpressionResult analyzeExpression(Node& node);
        i64 calculate(i64 left, i64 right, BinaryOperator);
        BuiltinType inferType(BuiltinType left, BuiltinType right, BinaryOperator);
        NodePointer insertImplicitCast(NodePointer value, const ExpressionResult& from, BuiltinType to);
        bool allowImplicitConversion(BuiltinType from, BuiltinType to);
        bool matchesFunction(const Function& function, const std::vector<BuiltinType>& args);

        SymbolTable variables;
        FunctionTable functions;
    };
} // namespace yuzu