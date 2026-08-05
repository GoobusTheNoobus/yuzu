#pragma once
#include "core/core.hpp"
#include "lexer/token.hpp"

namespace yuzu {
    class Lexer {
      public:
        Lexer() = default;
        TokenList tokenize(const std::string& source);

      private:
        std::string source;
        usize index{0};

        inline char current() const {
            return source.at(index);
        }

        inline char peek() const {
            return source.at(index + 1);
        }

        inline bool end() const {
            return index >= source.size();
        }

        inline void consume() {
            ++index;
        }

        std::string readWord();
        TokenType inferTypeFromWord(const std::string& word);
        TokenType inferTypeFromOperator(const std::string& word);
        void tokenizeString(TokenList& list);
        void tokenizeNumber(TokenList& list);
        void tokenizeChar(TokenList& list);
        void tokenizeOperator(TokenList& list);
    };
} // namespace yuzu