#include "diagnostic/error.hpp"
#include <iostream>

namespace yuzu::diagnostic {

    constexpr std::string_view errorTypeToString[] = {
        "SyntaxError", "FileNotOpenError", "InternalError", "NameError", "TypeError", "IntegerError"};

    void throwError(ErrorType err, const std::string& msg) {
        std::cerr << Red << "Yuzu." << errorTypeToString[(int)err] << ": " << msg << std::endl << Reset;
        std::exit(EXIT_FAILURE);
    }

} // namespace yuzu::diagnostic