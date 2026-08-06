#include "cli/file.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/analysis.hpp"
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

    std::cout << "DEBUG: \n------------------------------------------\n"
              << fileContent << std::endl
              << "------------------------------------------" << std::endl;

    Lexer lexer;
    auto tokens = lexer.tokenize(fileContent);
    Parser parser;
    auto node = parser.parse(tokens);
    Analysis analyzer;
    analyzer.analyze(static_cast<Program&>(*node));
    node->print(0);

    return EXIT_SUCCESS;
}