#include "core/core.hpp"

namespace yuzu::diagnostic {

    enum class ErrorType {
        SyntaxError,
        FileNotOpenError,
        InternalError,
        NameError,
        TypeError,
        IntegerError,
    };

    void throwError(ErrorType err, const std::string& msg);

} // namespace yuzu::diagnostic