# Yuzu

## What is it?
Yuzu is a LLVM based compiled programming language written in C++20. I made it
for coding practice. It is WIP, and far from being a functional compiler.

## Building
Requirements:
* C++20 compiler
* CMake
* Ninja
* LLVM libraries

To configure, navigate to project root and run
`cmake -B build -G Ninja`

To build, run
`cmake --build build`

## Usage
To get version, run `yuzu --version`.

To compile a file, run `yuzu <program.yz>`
