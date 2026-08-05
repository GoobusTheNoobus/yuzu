#pragma once
#include "lexer/lexer.hpp"
#include "parser/node.hpp"

namespace yuzu {
    class Parser {
      public:
        Parser() = default;
        NodePointer parse(TokenList& list);

      private:
        TokenList tokens;
        usize index{0};

        // Same functions as the lexer, just different type
        inline const Token& current() const {
            return tokens.at(index);
        }

        inline const Token& peek() const {
            return tokens.at(index + 1);
        }

        inline bool end() const {
            return index >= tokens.size() || current().type == TokenType::EndOfFile;
        }

        inline void consume() {
            ++index;
        }

        // Statements
        NodePointer parseStatement();
        NodePointer parseDeclaration();
        NodePointer parseAssignment();
        NodePointer parseReturn();

        // Expression
        NodePointer parseExpression() {
            // This function is just a wrapper for the lowest precedence operation
            // support at the moment
            parseAdditive();
        }

        NodePointer parsePrimary();
        NodePointer parsePostfix();
        NodePointer parseMultiplicative();
        NodePointer parseAdditive();
    };
} // namespace yuzu