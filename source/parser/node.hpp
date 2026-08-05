#pragma once
#include "core/core.hpp"
#include "core/types.hpp"
#include <memory>
#include <vector>

namespace yuzu {

    enum class NodeType {
        None,

        Program,
        Block,

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

    // Indexed, of course, by the enum number
    constexpr std::string_view nodeTypeToString[] = {
        "None",
        "Program",
        "Block",
        "IntegerLiteral",
        "StringLiteral",
        "CharacterLiteral",
        "FloatLiteral",
        "BooleanLiteral",
        "Identifier",
        "VariableDeclaration",
        "VariableAssignment",
        "ReturnStatement",
        "IfStatement",
        "WhileLoop",
        "ForLoop",
        "BuiltinType",
        "BinaryExpression",
        "UnaryExpression",
        "FunctionDefinition",
        "FunctionCall",
    };

    // Here lies our collection of AST node types

    struct Node;
    using NodePointer = std::unique_ptr<Node>;

    struct Node {
        NodeType type{NodeType::None};

        // Virtual dihstructor because inheritances have
        // different size
        Node(NodeType type) : type(type) {}

        virtual ~Node() = default;
    };

    struct Program : Node {
        std::vector<NodePointer> children;

        Program() : Node(NodeType::Program) {}
    };

    struct Block : Node {
        std::vector<NodePointer> children;

        Block() : Node(NodeType::Block) {}
    };

    struct IntegerLiteral : Node {
        i64 data;

        IntegerLiteral(i64 data) : Node(NodeType::IntegerLiteral), data(data) {}
    };

    struct StringLiteral : Node {
        std::string data;

        StringLiteral(std::string& data) : Node(NodeType::StringLiteral), data(data) {}
    };

    struct CharLiteral : Node {
        char data;

        CharLiteral(char data) : Node(NodeType::CharacterLiteral), data(data) {}
    };

    struct FloatLiteral : Node {
        f64 data;

        FloatLiteral(f64 data) : Node(NodeType::FloatLiteral), data(data) {}
    };

    struct BoolLiteral : Node {
        bool data;

        BoolLiteral(bool data) : Node(NodeType::BooleanLiteral), data(data) {}
    };

    struct Identifier : Node {
        std::string name;

        Identifier(std::string& name) : Node(NodeType::Identifier), name(name) {}
    };

    struct VarDecleration : Node {
        std::string name;
        NodePointer value;
        BuiltinType type{BuiltinType::Auto};

        VarDecleration(std::string& name, NodePointer& value, BuiltinType type)
            : Node(NodeType::VariableDeclaration), name(name), value(std::move(value)), type(type) {}
    };

    struct VarAssignment : Node {
        std::string name;
        NodePointer value;

        VarAssignment(std::string& name, NodePointer& value) : Node(NodeType::VariableAssignment), name(name), value(std::move(value)) {}
    };

    struct ReturnStatement : Node {
        NodePointer value;

        ReturnStatement(NodePointer& value) : Node(NodeType::ReturnStatement), value(std::move(value)) {}
    };

    struct TypeNode : Node {
        BuiltinType type;

        TypeNode(BuiltinType type) : Node(NodeType::BuiltinType), type(type) {}
    };

    struct BinaryNode : Node {
        NodePointer left, right;
        BinaryOperator op;

        BinaryNode(NodePointer& left, NodePointer& right, BinaryOperator op)
            : Node(NodeType::BinaryExpression), left(std::move(left)), right(std::move(right)), op(op) {}
    };

    struct FunctionCallNode : Node {
        std::string name;
        std::vector<NodePointer> children;

        FunctionCallNode(std::string& name, std::vector<NodePointer>& children)
            : Node(NodeType::FunctionCall), name(name), children(std::move(children)) {}
    };

    struct FunctionDefinitionNode : Node {
        std::string name;
        Block definition;
        bool isNative{false}; // defined by the compiler

        FunctionDefinitionNode(std::string& name, Block& block)
            : Node(NodeType::FunctionDefinition), name(name), definition(std::move(block)) {}
    };

} // namespace yuzu