#include "parser/parser.hpp"
#include "diagnostic/error.hpp"
#include <charconv>

namespace yuzu {

    namespace {
        BuiltinType tokenToBuiltinType(TokenType tok) {
            return (BuiltinType)((int)tok - (int)TokenType::TypeI8 + (int)BuiltinType::I8);
        }
    } // namespace

    NodePointer Parser::parse(TokenList& tokens) {
        this->tokens = tokens;
        auto program = std::make_unique<Program>();

        while (!end()) {
            if (current().type == TokenType::SemiColon) {
                consume();
                continue;
            }

            program->children.push_back(parseStatement());
        }

        return program;
    }

    NodePointer Parser::parseStatement() {
        NodePointer node = nullptr;

        switch (current().type) {
        case TokenType::KeywordLet: {
            node = parseDeclaration();
            break;
        }
        case TokenType::KeywordReturn: {
            node = parseReturn();
            break;
        }
        case TokenType::Identifier: {
            if (peek().type == TokenType::OperatorEqual) {
                node = parseAssignment();
                break;
            }

            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unexpected identifier");
        }
        case TokenType::KeywordExit: {
            consume();
            // std::cout << current();
            auto expr = parseExpression();
            if (!expr) {
                std::cout << "no\n";
                std::exit(-1);
            }

            node = std::make_unique<Exit>(std::move(expr));
            break;
        }
        default:
            std::cerr << tokenTypeToString[(int)current().type];
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected statement");
        }

        if (end() || current().type != TokenType::SemiColon) {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected ';'");
        }

        consume();

        return node;
    }

    NodePointer Parser::parseDeclaration() {
        consume();

        BuiltinType type = BuiltinType::Auto;

        auto& identifier = current();
        if (identifier.type != TokenType::Identifier) {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected an identifier");
        }

        consume();

        if (current().type == TokenType::Colon) {
            consume();

            if (!isType(current().type)) {
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected a type name");
            }

            type = tokenToBuiltinType(current().type);

            consume();
        }

        if (current().type == TokenType::OperatorEqual) {
            consume();

            if (current().type == TokenType::SemiColon) {
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected a value");
            }
            auto value = parseExpression();

            return std::make_unique<VarDecleration>(identifier.value, std::move(value), type);
        }

        diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected '=' for variable declaration");
        return nullptr;
    }

    NodePointer Parser::parseReturn() {
        consume();

        return std::make_unique<ReturnStatement>(parseExpression());
    }

    NodePointer Parser::parseAssignment() {
        std::string name = current().value;
        consume();

        if (current().type != TokenType::OperatorEqual) {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected '=' for variable assignment");
        }

        consume();

        return std::make_unique<VarAssignment>(name, parseExpression());
    }

    NodePointer Parser::parseAdditive() {
        auto left = parseMultiplicative();

        while (!end() && (current().type == TokenType::OperatorPlus || current().type == TokenType::OperatorMinus)) {
            BinaryOperator op = (current().type == TokenType::OperatorPlus) ? BinaryOperator::Plus : BinaryOperator::Minus;
            consume();

            auto right = parseMultiplicative();

            left = std::make_unique<BinaryNode>(std::move(left), std::move(right), op);
        }

        return left;
    }

    NodePointer Parser::parseMultiplicative() {
        auto left = parsePostfix();

        while (!end() && (current().type == TokenType::OperatorStar || current().type == TokenType::OperatorSlash ||
                          current().type == TokenType::OperatorPercent)) {
            BinaryOperator op;

            switch (current().type) {
            case TokenType::OperatorStar:
                op = BinaryOperator::Star;
                break;

            case TokenType::OperatorSlash:
                op = BinaryOperator::Slash;
                break;

            case TokenType::OperatorPercent:
                op = BinaryOperator::Percent;
                break;

            default:
                break;
            }

            consume();

            auto right = parsePostfix();

            left = std::make_unique<BinaryNode>(std::move(left), std::move(right), op);
        }

        return left;
    }

    NodePointer Parser::parsePostfix() {
        auto node = parsePrimary();

        if (node->type == NodeType::Identifier && current().type == TokenType::LeftParen) {
            consume();

            // Parse individual arguments
            std::vector<NodePointer> params;
            if (!(current().type == TokenType::RightParen)) {
                while (true) {
                    params.push_back(parseExpression());

                    if (current().type == TokenType::Comma) {
                        consume();
                        continue;
                    }

                    break;
                }
            }

            if (end() || current().type != TokenType::RightParen) {
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected ')' terminating argument list");
            }

            consume();

            auto& idNode = static_cast<Identifier&>(*node);
            auto functionCallNode = std::make_unique<FunctionCallNode>(idNode.name, params);
            return functionCallNode;
        }

        return node;
    }

    NodePointer Parser::parsePrimary() {
        const Token& token = current();
        consume();

        switch (token.type) {
        case TokenType::IntegerLiteral: {
            i64 value = 0;

            auto [ptr, ec] = std::from_chars(token.value.data(), token.value.data() + token.value.size(), value);

            if (ec == std::errc::result_out_of_range) {
                diagnostic::throwError(diagnostic::ErrorType::IntegerError, "Integer literal '" + token.value + "' is out of range");
            }

            return std::make_unique<IntegerLiteral>(value);
        }

        case TokenType::FloatLiteral: {
            return std::make_unique<FloatLiteral>(std::stof(token.value));
        }

        case TokenType::StringLiteral: {
            return std::make_unique<StringLiteral>(token.value);
        }

        case TokenType::CharacterLiteral: {
            return std::make_unique<CharLiteral>(token.value[0]);
        }

        case TokenType::BooleanLiteralFalse: {
            return std::make_unique<BoolLiteral>(false);
        }

        case TokenType::BooleanLiteralTrue: {
            return std::make_unique<BoolLiteral>(true);
        }

        case TokenType::Identifier: {
            return std::make_unique<Identifier>(token.value);
        }

        case TokenType::LeftParen: {
            auto expr = parseExpression();

            if (end() || current().type != TokenType::RightParen) {
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Expected ')");
            }

            consume();
            return expr;
        }

        default: {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unexpected token '" + token.value + "'");
        }
        }

        return nullptr;
    }

} // namespace yuzu