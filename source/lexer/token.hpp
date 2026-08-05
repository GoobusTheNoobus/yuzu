#pragma once
#include "core/core.hpp"
#include <vector>
#include <iostream>

namespace yuzu {

    enum class TokenType : u8 {

        // Special
        EndOfFile,
        
        // Literal types
        IntegerLiteral,
        StringLiteral,
        CharacterLiteral,
        FloatLiteral,
        BooleanLiteralTrue,
        BooleanLiteralFalse,

        Identifier,

        // Keywords
        KeywordLet,
        KeywordFunc,
        KeywordReturn,
        KeywordIf,
        KeywordElse,
        KeywordWhile,
        KeywordFor,

        // Built-in types
        TypeI8,
        TypeI16,
        TypeI32,
        TypeI64,
        TypeF32,
        TypeF64,
        TypeBool,
        TypeChar,
        TypeString,
        TypeVoid,

        // Operators
        OperatorPlus,
        OperatorMinus,
        OperatorStar,
        OperatorSlash,
        OperatorPercent,
        OperatorEqual,

        // Seperators
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,

        Comma,
        Colon,
        SemiColon,
    };

    struct Token {
        String value;
        TokenType type;
    };

    // String representations, indexed by the integer value 
    // of given TokenType

    inline constexpr StringView tokenTypeToString[] = {
        "EndOfFile",
        "IntegerLiteral",
        "StringLiteral",
        "CharacterLiteral",
        "FloatLiteral",
        "BooleanLiteralTrue",
        "BooleanLiteralFalse",
        "Identifier",
        "KeywordLet",
        "KeywordFunc",
        "KeywordReturn",
        "KeywordIf",
        "KeywordElse",
        "KeywordWhile",
        "KeywordFor",
        "TypeI8",
        "TypeI16",
        "TypeI32",
        "TypeI64",
        "TypeF32",
        "TypeF64",
        "TypeBool",
        "TypeChar",
        "TypeString",
        "TypeVoid",
        "OperatorPlus",
        "OperatorMinus",
        "OperatorStar",
        "OperatorSlash",
        "OperatorPercent",
        "OperatorEqual",
        "LeftParen",
        "RightParen",
        "LeftBrace",
        "RightBrace",
        "Comma",
        "Colon",
        "SemiColon",
    };

    using TokenList = std::vector<Token>;

    inline std::ostream& operator<<(std::ostream& stream, const Token& token) {
        if (token.value.empty()) 
            stream << "{ " << tokenTypeToString[(int)token.type] << " }";
        else 
            stream << "{ \"" << token.value << "\", " << tokenTypeToString[(int)token.type] << " }";
        
        return stream;
    }

    inline std::ostream& operator<<(std::ostream& stream, const TokenList& tokens) {
        for (auto& token : tokens) {
            stream << token << std::endl;
        }
        return stream;
    }
}