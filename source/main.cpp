#include <iostream>
#include "cli/file.hpp"
#include "lexer/lexer.hpp"

using namespace yuzu;

int main(int argc, char* argv[]) {

    if (argc <= 1) {
        std::cout << "Yuzu is a LLVM-based compiled programming language.\n";
        std::cout << "Usage: yuzu <file.yz>";
        return EXIT_SUCCESS;
    }

    // Read given file
    String fileContent = readFile(argv[1]);

    Lexer lexer;
    auto tokens = lexer.tokenize(fileContent);

    std::cout << tokens;

    return EXIT_SUCCESS;
}