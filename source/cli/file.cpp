#include "cli/file.hpp"
#include "diagnostic/error.hpp"
#include <fstream>
#include <sstream>

namespace yuzu {

    std::string readFile(const char path[]) {
        std::fstream input(path);
        if (!input.is_open()) {
            diagnostic::throwError(diagnostic::ErrorType::FileNotOpenError, "Cannot open file '" + std::string(path) + "'");
        }

        std::stringstream stream;
        stream << input.rdbuf();

        return stream.str();
    }

} // namespace yuzu