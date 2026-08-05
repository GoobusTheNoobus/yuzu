#include "core/core.hpp"

namespace yuzu::diagnostic {

    enum class ErrorType {
        SyntaxError,
        FileNotOpenError,
        InternalError,
    };

    void throwError(ErrorType err, const String& msg);
}