cmake_minimum_required(VERSION 3.20)

project(Yuzu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LLVM REQUIRED CONFIG)

add_executable(yuzu
    source/main.cpp
    source/cli/file.cpp
    source/diagnostic/error.cpp
    source/lexer/lexer.cpp
    source/parser/parser.cpp
)

target_include_directories(yuzu PRIVATE ${LLVM_INCLUDE_DIRS} source)

llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
)

target_link_libraries(yuzu PRIVATE ${LLVM_LIBS})