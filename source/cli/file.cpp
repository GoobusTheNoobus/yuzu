#include "cli/file.hpp"
#include "diagnostic/error.hpp"
#include <fstream>
#include <sstream>

namespace yuzu {
    String readFile(const char path[]) {
        std::fstream input(path);
        if (!input.is_open()) {
            diagnostic::throwError(diagnostic::ErrorType::FileNotOpenError, "Cannot open file '" + String(path) + "'");
            return "";
        }

        std::stringstream stream;
        stream << input.rdbuf();

        return stream.str();
    }
}