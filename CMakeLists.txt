cmake_minimum_required(VERSION 3.10)
project(dragon-lox)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(/Users/neeilanselvalingam/Desktop/llvm/llvm/include)

add_executable(dragon-lox
        include/expr.hpp
        include/lox.hpp
        include/scanner.hpp
        include/token.hpp
        src/cli.cpp
        src/lox.cpp
        src/scanner.cpp
        src/token.cpp
        include/visitor.h
        src/parser.cpp
        include/parser.h
        src/ast_printer.cpp
        include/ast_printer.hpp
        include/visitable_types.hpp
        include/runtime_err.hpp src/codegen.cpp include/codegen.hpp)
