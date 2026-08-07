cmake_minimum_required(VERSION 3.20)

project(Yuzu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)

add_executable(yuzu
    source/main.cpp
    source/file/file.cpp
    source/diagnostic/error.cpp
    source/lexer/lexer.cpp
    source/parser/parser.cpp
    source/semantic/analysis.cpp
    source/semantic/functions.cpp
    source/semantic/symbols.cpp
    source/codegen/codegen.cpp
)

target_include_directories(yuzu PRIVATE ${LLVM_INCLUDE_DIRS} source)

llvm_map_components_to_libnames(LLVM_LIBS
    Core
    Support
    Target
    TargetParser
    MC
    X86CodeGen
    X86AsmParser
    X86Desc
    X86Info
    AsmPrinter
    CodeGen
    ScalarOpts
    InstCombine
    TransformUtils
    Analysis
    Passes
)


target_link_libraries(yuzu PRIVATE ${LLVM_LIBS})