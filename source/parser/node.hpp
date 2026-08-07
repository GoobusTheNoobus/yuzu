#pragma once
#include "core/core.hpp"
#include "core/types.hpp"
#include <iostream>
#include <memory>
#include <vector>


namespace yuzu {

    enum class NodeType {
        None,

        Program,
        Block,
        Cast,
        Exit,

        IntegerLiteral,
        StringLiteral,
        CharacterLiteral,
        FloatLiteral,
        BooleanLiteral,

        Identifier,

        VariableDeclaration,
        VariableAssignment,
        ReturnStatement,
        IfStatement, // unimplemented
        WhileLoop,   // unimplemented
        ForLoop,     // unimplemented

        BuiltinType,
        BinaryExpression,
        UnaryExpression, // unimplemented

        FunctionDefinition,
        FunctionCall,

    };

    enum class BinaryOperator {
        Plus,
        Minus,
        Star,
        Slash,
        Percent,
    };

    constexpr std::string_view binaryOperatorToString[] = {
        "+",
        "-",
        "*",
        "/",
        "%",
    };

    // Here lies our collection of AST node types

    struct Node;
    using NodePointer = std::unique_ptr<Node>;

    struct Node {
        NodeType type{NodeType::None};
        BuiltinType resolvedType{BuiltinType::Auto};

        // Virtual dihstructor because inheritances have
        // different size
        Node(NodeType type) : type(type) {}

        virtual ~Node() = default;
        virtual void print(int indent) const = 0;
    };

    struct Program : Node {
        std::vector<NodePointer> children;

        Program() : Node(NodeType::Program) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Root)" << std::endl;

            for (auto& pointer : children) {
                pointer->print(indent + 4);
                std::cout << std::endl;
            }
        }
    };

    struct Block : Node {
        std::vector<NodePointer> children;

        Block() : Node(NodeType::Block) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Block)" << std::endl;

            for (auto& pointer : children) {
                pointer->print(indent + 4);
                std::cout << std::endl;
            }
        }
    };

    struct IntegerLiteral : Node {
        i64 data;

        IntegerLiteral(i64 data) : Node(NodeType::IntegerLiteral), data(data) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Integer)" << data << std::endl;
        }
    };

    struct StringLiteral : Node {
        std::string data;

        StringLiteral(const std::string& data) : Node(NodeType::StringLiteral), data(data) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(String)\"" << data << "\"" << std::endl;
        }
    };

    struct CharLiteral : Node {
        char data;

        CharLiteral(char data) : Node(NodeType::CharacterLiteral), data(data) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Char)'" << data << "'" << std::endl;
        }
    };

    struct FloatLiteral : Node {
        f64 data;

        FloatLiteral(f64 data) : Node(NodeType::FloatLiteral), data(data) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Float)" << data << std::endl;
        }
    };

    struct BoolLiteral : Node {
        bool data;

        BoolLiteral(bool data) : Node(NodeType::BooleanLiteral), data(data) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Bool)" << (data ? "true" : "false") << std::endl;
        }
    };

    struct Identifier : Node {
        std::string name;

        Identifier(const std::string& name) : Node(NodeType::Identifier), name(name) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Identifier)" << name << '\n';
        }
    };

    struct VarDecleration : Node {
        std::string name;
        NodePointer value;
        BuiltinType varType{BuiltinType::Auto};

        VarDecleration(const std::string& name, NodePointer value, BuiltinType type)
            : Node(NodeType::VariableDeclaration), name(name), value(std::move(value)), varType(type) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(VariableDeclaration) " << name;

            if (varType != BuiltinType::Auto)
                std::cout << " : " << builtinTypeToString[(int)varType];

            std::cout << '\n';

            value->print(indent + 4);
        }
    };

    struct VarAssignment : Node {
        std::string name;
        NodePointer value;

        VarAssignment(const std::string& name, NodePointer value)
            : Node(NodeType::VariableAssignment), name(name), value(std::move(value)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(VariableAssignment) " << name << '\n';

            value->print(indent + 4);
        }
    };

    struct ReturnStatement : Node {
        NodePointer value;

        ReturnStatement(NodePointer value) : Node(NodeType::ReturnStatement), value(std::move(value)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Return)\n";

            value->print(indent + 4);
        }
    };

    struct TypeNode : Node {
        BuiltinType type;

        TypeNode(BuiltinType type) : Node(NodeType::BuiltinType), type(type) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Type) " << builtinTypeToString[(int)type] << '\n';
        }
    };

    struct BinaryNode : Node {
        NodePointer left, right;
        BinaryOperator op;

        BinaryNode(NodePointer left, NodePointer right, BinaryOperator op)
            : Node(NodeType::BinaryExpression), left(std::move(left)), right(std::move(right)), op(op) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(BinaryExpression) " << binaryOperatorToString[(int)op] << '\n';

            left->print(indent + 4);
            right->print(indent + 4);
        }
    };

    struct FunctionCallNode : Node {
        std::string name;
        std::vector<NodePointer> children;

        FunctionCallNode(std::string& name, std::vector<NodePointer>& children)
            : Node(NodeType::FunctionCall), name(name), children(std::move(children)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(FunctionCall) " << name << '\n';

            for (const auto& arg : children)
                arg->print(indent + 4);
        }
    };

    struct FunctionDefinitionNode : Node {
        std::string name;
        Block definition;
        bool isNative{false}; // defined by the compiler

        FunctionDefinitionNode(std::string& name, Block& block)
            : Node(NodeType::FunctionDefinition), name(name), definition(std::move(block)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(FunctionDefinition) " << name;

            if (isNative)
                std::cout << " [native]";

            std::cout << '\n';

            definition.print(indent + 4);
        }
    };

    struct Exit : Node {
        NodePointer code;

        Exit(NodePointer code) : Node(NodeType::Exit), code(std::move(code)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Exit) " << '\n';
            code->print(indent + 4);
        }
    };

    struct CastNode : Node {
        BuiltinType to;
        NodePointer thing;

        CastNode(BuiltinType to, NodePointer thing) : Node(NodeType::Cast), to(to), thing(std::move(thing)) {}

        void print(int indent) const override {
            std::cout << std::string(indent, ' ') << "(Cast)" << builtinTypeToString[(int)to] << std::endl;
            thing->print(indent + 4);
        }
    };

} // namespace yuzu