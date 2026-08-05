#include "diagnostic/error.hpp"
#include <iostream>

namespace yuzu::diagnostic {

    constexpr StringView errorTypeToString[] = {
        "SyntaxError",
        "FileNotOpenError",
        "InternalError"
    };

    void throwError(ErrorType err, const String& msg) {
        std::cerr << Red << "Yuzu." << errorTypeToString[(int)err] << ": " << msg << std::endl << Reset;
        std::exit(EXIT_FAILURE);
    }
}