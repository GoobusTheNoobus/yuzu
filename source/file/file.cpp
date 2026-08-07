#include "file/file.hpp"
#include "diagnostic/error.hpp"

#include <fstream>
#include <sstream>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>

namespace yuzu {

    std::string readFile(const char path[]) {
        std::fstream input(path);
        if (!input.is_open())
            diagnostic::throwError(diagnostic::ErrorType::FileNotOpenError, "Cannot open file '" + std::string(path) + "'");

        std::stringstream stream;
        stream << input.rdbuf();
        return stream.str();
    }

    void writeIRFile(const char name[], const llvm::Module* module) {
        std::error_code error;
        llvm::raw_fd_ostream output(name, error);

        if (error)
            diagnostic::throwError(diagnostic::ErrorType::FileNotOpenError, "Cannot write IR file. Message: " + error.message());

        module->print(output, nullptr);
    }

    void writeObjectFile(const char name[], llvm::Module* module) {
        // Target setup
        auto triple = llvm::Triple(llvm::sys::getDefaultTargetTriple());
        module->setTargetTriple(triple);

        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget(triple, error);
        if (!target) {
            diagnostic::throwError(diagnostic::ErrorType::InternalError, "Failed to lookup target: " + error);
        }

        auto cpu = "generic";
        auto features = "";

        llvm::TargetOptions opt;
        auto rm = std::optional<llvm::Reloc::Model>();
        auto targetMachine = target->createTargetMachine(triple, cpu, features, opt, rm);

        module->setDataLayout(targetMachine->createDataLayout());

        // Optimizations
        {
            llvm::PassBuilder passBuilder;

            llvm::LoopAnalysisManager lam;
            llvm::FunctionAnalysisManager fam;
            llvm::CGSCCAnalysisManager cgam;
            llvm::ModuleAnalysisManager mam;

            passBuilder.registerModuleAnalyses(mam);
            passBuilder.registerCGSCCAnalyses(cgam);
            passBuilder.registerFunctionAnalyses(fam);
            passBuilder.registerLoopAnalyses(lam);
            passBuilder.crossRegisterProxies(lam, fam, cgam, mam);

            llvm::ModulePassManager mpm = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);

            mpm.run(*module, mam);
        }

        // Emit object file
        std::error_code ec;
        llvm::raw_fd_ostream dest(name, ec, llvm::sys::fs::OF_None);
        if (ec) {
            diagnostic::throwError(diagnostic::ErrorType::FileNotOpenError, "Could not open file: " + ec.message());
        }

        llvm::legacy::PassManager pass;
        auto fileType = llvm::CodeGenFileType::ObjectFile;

        if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
            diagnostic::throwError(diagnostic::ErrorType::InternalError, "TargetMachine can't emit a file of this type");
        }

        pass.run(*module);
        dest.flush();
    }

    void link(const char* objectFile, const char* outputExecutable) {
        auto compiler = llvm::sys::findProgramByName("g++");

        if (!compiler) {
            diagnostic::throwError(diagnostic::ErrorType::InternalError, "Could not find g++ in PATH");
        }

        std::vector<llvm::StringRef> args = {*compiler, objectFile, "-o", outputExecutable, "-O2"};

        std::string error;
        int result = llvm::sys::ExecuteAndWait(*compiler, args, std::nullopt, {}, 0, 0, &error);

        if (result != 0) {
            diagnostic::throwError(diagnostic::ErrorType::InternalError, "Linking failed: " + error);
        }
    }

} // namespace yuzu