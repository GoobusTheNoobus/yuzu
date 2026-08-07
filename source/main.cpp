#include "cli/file.hpp"
#include "codegen/codegen.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/analysis.hpp"
#include <chrono>
#include <iostream>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

using namespace yuzu;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Usage: " << std::endl;
        std::cerr << "yuzu <program.yz>" << std::endl;
        return EXIT_FAILURE;
    }

    if (std::string(argv[1]) == "--version") {
        std::cout << "Yuzu version 0.3.0" << std::endl;
        return EXIT_SUCCESS;
    }

    // Read given file
    std::string fileContent = readFile(argv[1]);

    /*
    std::cout << "DEBUG: \n------------------------------------------\n"
              << fileContent << std::endl
              << "------------------------------------------" << std::endl;
    */

    std::cout << "[1/4] Tokenizing code";
    auto start = steady_clock::now();
    Lexer lexer;
    TokenList tokens = lexer.tokenize(fileContent);
    auto end = steady_clock::now();

    std::cout << "\r[1/4] Tokenizing code (finished in " << (duration_cast<microseconds>(end - start).count() / 1000.0) << " ms)"
              << std::endl;

    std::cout << "[2/4] Parsing code";
    start = steady_clock::now();
    Parser parser;
    NodePointer node = parser.parse(tokens);
    Program& program = static_cast<Program&>(*node);

    end = steady_clock::now();

    std::cout << "\r[2/4] Parsing code (finished in " << (duration_cast<microseconds>(end - start).count() / 1000.0) << " ms)" << std::endl;

    std::cout << "[3/4] Performing semantic analysis";
    start = steady_clock::now();
    Analysis analyzer;
    analyzer.analyze(program);

    end = steady_clock::now();

    std::cout << "\r[3/4] Performing semantic analysis (finished in " << (duration_cast<microseconds>(end - start).count() / 1000.0)
              << " ms)" << std::endl;

    std::cout << "[4/4] Generating IR file";
    start = steady_clock::now();

    CodeGen generator;
    llvm::Module* module = generator.generate(program);
    std::error_code error;

    llvm::raw_fd_ostream output("temp.ll", error);

    if (error) {
        llvm::errs() << "Could not open file: " << error.message() << "\n";
        return EXIT_FAILURE;
    }

    module->print(output, nullptr);
    end = steady_clock::now();

    std::cout << "\r[4/4] Generating IR file (finished in " << (duration_cast<microseconds>(end - start) / 1000.0).count() << " ms)"
              << std::endl;

    return EXIT_SUCCESS;
}