#include "cli/file.hpp"
#include "lexer/lexer.hpp"
#include <iostream>

using namespace yuzu;

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: " << std::endl;
        std::cerr << "yuzu <program.yz>" << std::endl;
        return EXIT_FAILURE;
    }

    if (argv[1] == "--version") {
        std::cout << "Yuzu version 0.3.0" << std::endl;
        return EXIT_SUCCESS;
    }

    // Read given file
    std::string fileContent = readFile(argv[1]);

    Lexer lexer;
    auto tokens = lexer.tokenize(fileContent);

    std::cout << tokens;

    return EXIT_SUCCESS;
}