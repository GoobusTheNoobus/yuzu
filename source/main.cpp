#include "codegen/codegen.hpp"
#include "file/file.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/analysis.hpp"
#include <iostream>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

using namespace yuzu;

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

    // Read file
    std::string fileName = argv[1];
    std::string fileContent = readFile(fileName.data());

    // Lexical analysis
    std::cout << "[1/4] Parsing code" << std::endl;
    Lexer lexer;
    TokenList tokens = lexer.tokenize(fileContent);

    // Parse tree
    Parser parser;
    NodePointer node = parser.parse(tokens);
    Program& program = static_cast<Program&>(*node);

    // Semantic analysis
    std::cout << "[2/4] Analyzing code" << std::endl;
    Analysis analyzer;
    analyzer.analyze(program);

    // LLVM codegen & object file
    std::cout << "[3/4] Generating object file" << std::endl;

    // Initialize llvm shit
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    CodeGen generator;
    llvm::Module* _module = generator.generate(program);

    writeObjectFile("obj.o", _module);

    std::cout << "[4/4] Linking output executable" << std::endl;

    link("obj.o", "program.exe");

    return EXIT_SUCCESS;
}