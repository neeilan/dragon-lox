#include "expr.hpp"
#include "lox.hpp"
#include "parser.h"
#include "stmt.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include "runtime_err.hpp"
#include "codegen.hpp"
#include "resolver.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <ast_printer.hpp>


bool Lox::had_error = false;
bool Lox::had_runtime_error = false;


void Lox::run_file(const char *path) {
    const std::ifstream file(path);
    std::stringstream src_buffer;

    src_buffer << file.rdbuf();

    run(src_buffer.str());

    if (had_error) exit(65);  // data format error
    if (had_runtime_error) exit(70);
}

void Lox::run_prompt() {
    std::string curr_line;

    while (true) {
        had_error = false;  // reset error status
        getline(std::cin, curr_line);

        std::cout << "> ";
        run(curr_line);
        std::cout << std::endl;
    }
}

void Lox::run(const std::string& source) {
    Scanner scanner(source);
    const std::vector<Token> tokens = scanner.scan_tokens();

    Parser parser(tokens);
    std::vector<Stmt *> statements = parser.parse();

    CodeGenerator codegen;
    Resolver resolver(codegen);

    resolver.resolve(statements);
    codegen.generate(statements);

    // Stop if there was a parsing error.
//    if (had_error) return;
}


void Lox::error(int line, const std::string& message) {
    report(line, "", message);
}

void Lox::error(Token token, const std::string& message) {
    if (token.type == END_OF_FILE) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void Lox::runtime_error(RuntimeErr err) {
    std::cout << "[line " << err.token.line << "] "
              << err.what()
              << std::endl;

    had_runtime_error = true;

}


// Private

void Lox::report(int line,
                 const std::string& occurrence,
                 const std::string& message) {
    std::cout << "[line " << line << "] Error: "
              << occurrence << " : " << message
              << std::endl;

    had_error = true;
}
