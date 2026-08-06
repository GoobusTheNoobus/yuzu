#include "semantic/analysis.hpp"
#include "diagnostic/error.hpp"

namespace yuzu {

    void Analysis::analyze(Program& program) {
        for (auto& child : program.children) {
            analyzeStatement(*child);
        }
    }

    void Analysis::analyzeStatement(Node& node) {
        switch (node.type) {
        case NodeType::VariableDeclaration: {
            analyzeDeclaration(static_cast<VarDecleration&>(node));
            break;
        }
        case NodeType::VariableAssignment: {
            analyzeAssignment(static_cast<VarAssignment&>(node));
            break;
        }
        case NodeType::FunctionCall: {
            analyzeCall(static_cast<FunctionCallNode&>(node));
            break;
        }
        case NodeType::Exit: {
            auto& exit = static_cast<Exit&>(node);
            auto analysis = analyzeExpression(*exit.code);

            if (!isInteger(analysis.resolvedType)) {
                diagnostic::throwError(diagnostic::ErrorType::TypeError, "Expected integer type for exit code");
            }
            break;
        }

        default: {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unknown statement");
        }
        }
    }

    void Analysis::analyzeDeclaration(VarDecleration& varDecl) {
        auto analysis = analyzeExpression(*varDecl.value);

        if (variables.lookup(varDecl.name)) {
            diagnostic::throwError(diagnostic::ErrorType::NameError, "Variable '" + varDecl.name + "' already defined");
        }

        if (varDecl.varType == BuiltinType::Auto)
            varDecl.varType = analysis.resolvedType;

        varDecl.value = insertImplicitCast(std::move(varDecl.value), analysis, varDecl.varType);

        varDecl.resolvedType = varDecl.varType;

        variables.define({varDecl.name, varDecl.resolvedType});
    }

    void Analysis::analyzeAssignment(VarAssignment& varAss) {
        auto lookup = variables.lookup(varAss.name);

        if (!lookup) {
            diagnostic::throwError(diagnostic::ErrorType::NameError, "Use of undeclared variable '" + varAss.name + "'");
        }

        auto analysis = analyzeExpression(*varAss.value);

        varAss.value = insertImplicitCast(std::move(varAss.value), analysis, lookup->type);

        varAss.resolvedType = lookup->type;
    }

    void Analysis::analyzeCall(FunctionCallNode& callNode) {
        auto lookup = functions.lookup(callNode.name);

        if (!lookup) {
            diagnostic::throwError(diagnostic::ErrorType::NameError, "Use of undefined function '" + callNode.name + "'");
        }

        std::vector<BuiltinType> args;
        for (auto& child : callNode.children) {
            args.push_back(analyzeExpression(*child).resolvedType);
        }

        if (!matchesFunction(*lookup, args)) {
            diagnostic::throwError(diagnostic::ErrorType::TypeError, "No overload of '" + callNode.name + "' matches the argument list");
        }
    }

    ExpressionResult Analysis::analyzeExpression(Node& node) {
        switch (node.type) {
        case NodeType::IntegerLiteral: {
            return {BuiltinType::I64, true, static_cast<IntegerLiteral&>(node).data};
        }
        case NodeType::StringLiteral: {
            return {BuiltinType::String, false};
        }
        case NodeType::CharacterLiteral: {
            return {BuiltinType::Char, false};
        }
        case NodeType::FloatLiteral: {
            return {BuiltinType::F64, false};
        }
        case NodeType::BooleanLiteral: {
            return {BuiltinType::Bool, false};
        }
        case NodeType::Identifier: {
            auto& identifier = static_cast<Identifier&>(node);
            auto lookup = variables.lookup(identifier.name);

            if (!lookup) {
                diagnostic::throwError(diagnostic::ErrorType::NameError, "Unknown identifier '" + identifier.name + "'");
            }

            return {lookup->type, false};
        }
        case NodeType::FunctionCall: {
            auto& callNode = static_cast<FunctionCallNode&>(node);
            auto lookup = functions.lookup(callNode.name);

            if (!lookup) {
                diagnostic::throwError(diagnostic::ErrorType::NameError, "Use of undefined function '" + callNode.name + "'");
            }

            // Built argument list
            std::vector<BuiltinType> args;
            for (auto& child : callNode.children) {
                args.push_back(analyzeExpression(*child).resolvedType);
            }

            if (!matchesFunction(*lookup, args)) {
                diagnostic::throwError(diagnostic::ErrorType::TypeError,
                                       "No overload of '" + callNode.name + "' matches the argument list");
            }

            return {lookup->type, false};
        }
        case NodeType::BinaryExpression: {
            auto& binary = static_cast<BinaryNode&>(node);
            auto left = analyzeExpression(*binary.left);
            auto right = analyzeExpression(*binary.right);

            i64 result = calculate(left.constantInt, right.constantInt, binary.op);
            BuiltinType type = inferType(left.resolvedType, right.resolvedType, binary.op);
            bool isConstantInt = left.isConstantInt && right.isConstantInt;

            return {type, isConstantInt, isConstantInt ? calculate(left.constantInt, right.constantInt, binary.op) : 0};
        }

        default: {
            diagnostic::throwError(diagnostic::ErrorType::InternalError,
                                   "Unknown expression type " + std::string(builtinTypeToString[(int)node.type]));
            return {};
        }
        }
    }

    i64 Analysis::calculate(i64 left, i64 right, BinaryOperator op) {
        switch (op) {
        case BinaryOperator::Plus: {
            return left + right;
        }
        case BinaryOperator::Minus: {
            return left - right;
        }
        case BinaryOperator::Star: {
            return left * right;
        }
        case BinaryOperator::Slash: {
            return left / right;
        }
        case BinaryOperator::Percent: {
            return left % right;
        }
        default: {
            diagnostic::throwError(diagnostic::ErrorType::InternalError, "Unknown operator of id: " + (int)op);
            return 0;
        }
        }
    }

    BuiltinType Analysis::inferType(BuiltinType left, BuiltinType right, BinaryOperator op) {
        if (isInteger(left) && isInteger(right)) {
            return (BuiltinType)std::max(int(left), int(right));
        }

        if (isInteger(left) && isFloat(right)) {
            return right;
        }

        if (isInteger(right) && isFloat(left)) {
            return left;
        }

        if (isFloat(right) && isFloat(left)) {
            return (BuiltinType)std::max(int(left), int(right));
        }

        if (op == BinaryOperator::Plus && left == BuiltinType::String) {
            return BuiltinType::String;
        }

        diagnostic::throwError(diagnostic::ErrorType::TypeError,
                               "Invalid operands for binary operator " + std::string(binaryOperatorToString[(int)op]) +
                                   ". The operand types are: '" + std::string(builtinTypeToString[(int)left]) + "' and '" +
                                   std::string(builtinTypeToString[(int)right]) + "'");
        return BuiltinType::None;
    }

    NodePointer Analysis::insertImplicitCast(NodePointer value, const ExpressionResult& from, BuiltinType to) {
        if (from.resolvedType == to)
            return value;

        if (from.isConstantInt) {
            auto smallest = smallestFittingInt(from.constantInt);

            if (smallest <= to) {
                return std::make_unique<CastNode>(to, std::move(value));
            }

            diagnostic::throwError(diagnostic::ErrorType::TypeError,
                                   "Integer literal '" + std::to_string(from.constantInt) + "' cannot fit in type " +
                                       std::string(builtinTypeToString[(int)to]));
        }

        if (allowImplicitConversion(from.resolvedType, to)) {
            return std::make_unique<CastNode>(to, std::move(value));
        }

        diagnostic::throwError(diagnostic::ErrorType::TypeError,
                               "Invalid conversion from '" + std::string(builtinTypeToString[(int)from.resolvedType]) + "' to '" +
                                   std::string(builtinTypeToString[(int)to]) + "'");
        return nullptr;
    }

    bool Analysis::allowImplicitConversion(BuiltinType from, BuiltinType to) {
        if (from == to)
            return true;

        if (isInteger(from) && isInteger(to))
            return from < to;

        if (isInteger(from) && isFloat(to))
            return true;

        if (isFloat(from) && isFloat(to))
            return from < to;

        return false;
    }

    bool Analysis::matchesFunction(const Function& function, const std::vector<BuiltinType>& args) {
        if (function.argList.size() != args.size())
            return false;

        for (size_t i = 0; i < args.size(); i++) {
            if (function.argList[i] != args[i])
                return false;
        }

        return true;
    }
} // namespace yuzu