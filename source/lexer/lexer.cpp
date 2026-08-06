#include "lexer/lexer.hpp"
#include "diagnostic/error.hpp"

namespace yuzu {
    namespace {
        char escapeString(char c) {
            switch (c) {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            case '\\':
                return '\\';
            case '"':
                return '"';
            case '0':
                return '\0';
            default:
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unknown escape character '" + std::string(1, c) + "'");
            }

            return '\0';
        }

        char escapeChar(char c) {
            switch (c) {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case 'r':
                return '\r';
            case '\\':
                return '\\';
            case '\'':
                return '\'';
            case '0':
                return '\0';
            default:
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unknown escape character '" + std::string(1, c) + "'");
            }

            return '\0';
        }
    } // namespace

    TokenList Lexer::tokenize(const std::string& source) {
        this->source = source;

        TokenList tokens;

        while (!end()) {
            char c = current();

            if (std::isspace(static_cast<unsigned char>(c))) {
                consume();
                continue;
            }

            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                auto value = readWord();
                tokens.push_back({value, inferTypeFromWord(value)});
                continue;
            }

            if (c == '#') {
                while (!end() && current() != '\n') {
                    consume();
                }

                continue;
            }

            if (c == '"') {
                tokenizeString(tokens);
                continue;
            }

            if (c == '\'') {
                tokenizeChar(tokens);
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(c))) {
                tokenizeNumber(tokens);
                continue;
            }

            // seperators
            TokenType type = TokenType::EndOfFile;
            if (c == '(')
                type = TokenType::LeftParen;
            else if (c == ')')
                type = TokenType::RightParen;
            else if (c == '{')
                type = TokenType::LeftBrace;
            else if (c == '}')
                type = TokenType::RightBrace;
            else if (c == ',')
                type = TokenType::Comma;
            else if (c == ':')
                type = TokenType::Colon;
            else if (c == ';')
                type = TokenType::SemiColon;

            if (type != TokenType::EndOfFile) {
                consume();
                tokens.push_back({std::string(1, c), type});
                continue;
            }

            tokenizeOperator(tokens);
        }

        tokens.push_back({"", TokenType::EndOfFile});
        return tokens;
    }

    std::string Lexer::readWord() {
        std::string word;

        while (!end() && (std::isalnum(static_cast<unsigned char>(current())) || current() == '_')) {
            word += current();
            consume();
        }

        return word;
    }

    TokenType Lexer::inferTypeFromWord(const std::string& word) {
        if (word == "exit")
            return TokenType::KeywordExit;
        if (word == "let")
            return TokenType::KeywordLet;
        if (word == "func")
            return TokenType::KeywordFunc;
        if (word == "return")
            return TokenType::KeywordReturn;
        if (word == "if")
            return TokenType::KeywordIf;
        if (word == "elseif")
            return TokenType::KeywordElseif;
        if (word == "else")
            return TokenType::KeywordElse;
        if (word == "while")
            return TokenType::KeywordWhile;
        if (word == "for")
            return TokenType::KeywordFor;
        if (word == "true")
            return TokenType::BooleanLiteralTrue;
        if (word == "false")
            return TokenType::BooleanLiteralFalse;
        if (word == "i8")
            return TokenType::TypeI8;
        if (word == "i16")
            return TokenType::TypeI16;
        if (word == "i32")
            return TokenType::TypeI32;
        if (word == "i64")
            return TokenType::TypeI64;
        if (word == "f32")
            return TokenType::TypeF32;
        if (word == "f64")
            return TokenType::TypeF64;
        if (word == "bool")
            return TokenType::TypeBool;
        if (word == "char")
            return TokenType::TypeChar;
        if (word == "string")
            return TokenType::TypeString;
        if (word == "void")
            return TokenType::TypeVoid;

        return TokenType::Identifier;
    }

    TokenType Lexer::inferTypeFromOperator(const std::string& word) {
        if (word == "+")
            return TokenType::OperatorPlus;
        if (word == "-")
            return TokenType::OperatorMinus;
        if (word == "*")
            return TokenType::OperatorStar;
        if (word == "/")
            return TokenType::OperatorSlash;
        if (word == "%")
            return TokenType::OperatorPercent;
        if (word == "=")
            return TokenType::OperatorEqual;

        diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unknown operator '" + word + "'");
        return TokenType::EndOfFile;
    }

    void Lexer::tokenizeString(TokenList& list) {
        std::string string;

        consume(); // consume "

        while (!end() && current() != '"') {
            char c = current();

            if (c != '\\') {
                string.push_back(c);
                consume();
                continue;
            }

            // escape character
            if (index >= source.size() - 1)
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unterminated string literal");

            consume(); // consume '\'

            char escape = current();
            string.push_back(escapeString(escape));
            consume();
        }

        if (current() != '"') {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unterminated string literal");
        }

        consume();

        list.push_back({string, TokenType::StringLiteral});
    }

    void Lexer::tokenizeChar(TokenList& list) {
        consume(); // consume '

        if (current() != '\\') {
            list.push_back({std::string(1, current()), TokenType::CharacterLiteral});
            consume();
        } else {
            consume(); // consume '\'

            if (end()) {
                diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unterminated character literal");
            }

            list.push_back({std::string(1, escapeChar(current())), TokenType::CharacterLiteral});
            consume();
        }

        if (current() != '\'') {
            diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Unterminated character literal");
        }

        consume();
    }

    void Lexer::tokenizeNumber(TokenList& list) {
        bool isFloat = false;
        std::string string;

        while (!end() && (std::isdigit(current()) || current() == '.')) {
            char c = current();
            consume();

            if (c == '.') {
                if (isFloat) {
                    diagnostic::throwError(diagnostic::ErrorType::SyntaxError, "Multiple '.' within float literal");
                }

                isFloat = true;
                string.push_back(c);
                continue;
            }

            string.push_back(c);
        }

        list.push_back({string, isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral});
    }

    void Lexer::tokenizeOperator(TokenList& list) {
        std::string string;

        while (!end() && !std::isalnum(current()) && !std::isspace(current()) && !(current() == '_') && !(current() == '#') &&
               !(current() == '\'') && !(current() == '"') && !(current() == '(') && !(current() == ')') && !(current() == '{') &&
               !(current() == '}') && !(current() == ',') && !(current() == ':') && !(current() == ';')) {
            string.push_back(current());
            consume();
        }

        TokenType type = inferTypeFromOperator(string);

        list.push_back({string, type});
    }
} // namespace yuzu