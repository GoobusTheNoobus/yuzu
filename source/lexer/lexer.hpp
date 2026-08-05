#pragma once
#include "core/core.hpp"
#include "lexer/token.hpp"

namespace yuzu {
    class Lexer {
    public:
        Lexer() = default;
        TokenList tokenize(const String& source);

    private:
        String source;
        usize index = 0;

        inline char current() const { return source.at(index); }
        inline char peek() const { return source.at(index + 1); }
        inline bool end() const { return index >= source.size(); }
        inline void consume() { ++index; }

        String readWord();
        TokenType inferTypeFromWord(const String& word);
        TokenType inferTypeFromOperator(const String& word);
        void tokenizeString(TokenList& list);
        void tokenizeNumber(TokenList& list);
        void tokenizeChar(TokenList& list);
        void tokenizeOperator(TokenList& list);
    };
}