#include "parser.h"
#include "lox.hpp"
#include "token.hpp"
#include "stmt.hpp"
#include "codegen.hpp"

#include <memory>
#include <vector>

#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

std::vector<Stmt*> Parser::parse() {
    std::vector<Stmt*> statements;

    while (!at_end()) {
        // try {
            statements.push_back(declaration());
        // } catch  (ParseErr) {
        //    synchronize();
        // }
    }

    return statements;
}


Stmt* Parser::declaration() {
    if (match({FUN})) return func_statement("function");
    if (match({CLASS})) return class_declaration();
    if (match({VAR})) {
        return var_declaration();
    } else {
        return statement();
    }
}

Stmt* Parser::var_declaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");
    Expr* initializer = new StrLiteral("nil", true);

    if (match({EQUAL})) {
        initializer = expression();
    }
    consume(SEMICOLON, "Expect ';' after variable declaration.");
    return new VarStmt(name, initializer);
}

Stmt* Parser::class_declaration() {
    Token name = consume(IDENTIFIER, "Expect class name.");

    Variable* superclass = nullptr;

    if (match({LESS})) {
        consume(IDENTIFIER, "Expect superclass name.");
        superclass = new Variable(previous());
    }

    consume(LEFT_BRACE, "Expect '{' before class body.");

    std::vector<Stmt*> methods;

    while (!check(RIGHT_BRACE) && !at_end()) {
        methods.push_back(func_statement("method"));
    }

    consume(RIGHT_BRACE, "Expect '}' after class body.");

    return new ClassStmt(name, superclass, methods);
}

Stmt* Parser::statement() {
    if (match({IF})) return if_statement();
    if (match({PRINT})) return print_statement();
    if (match({RETURN})) return return_statement();
    if (match({WHILE})) return  while_statement();
    if (match({FOR})) return for_statement();
    if (match({LEFT_BRACE})) return block_statement();


    return expression_statement();
}

Stmt* Parser::print_statement() {
    Expr* value = expression();
    consume(SEMICOLON, "Expect ';' after value.");
    return new PrintStmt(value);
}

Stmt* Parser::return_statement() {
    Token keyword = previous();
    Expr* value = nullptr;

    if (!check(SEMICOLON)) {
        value = expression();
    }

    consume(SEMICOLON, "Expect ';' after return value.");
    return new ReturnStmt(keyword, value);
}

Stmt* Parser::block_statement() {
    std::vector<Stmt*> stmts;

    while (!check(RIGHT_BRACE) && !at_end()) {
        stmts.push_back(declaration());
    }

    consume(RIGHT_BRACE, "Expect '}' after block.");

    return new BlockStmt(stmts);
}

Stmt* Parser::if_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after if condition.");

    Stmt* then_branch = statement();
    Stmt* else_branch = match({ELSE}) ? statement() : nullptr;

    return new IfStmt(condition, then_branch, else_branch);
}

Stmt* Parser::while_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(RIGHT_PAREN, "Expect ')' after condition.");

    Stmt* body = statement();
    return new WhileStmt(condition, body);
}

/*
 * Desugar for-loop into equivalent while-loop.
 * for-loop syntax: for (init_stmt? ; condition_expr? ; incr_stmt?) stmt;
 */
Stmt* Parser::for_statement() {
    consume(LEFT_PAREN, "Expect '(' after 'for'.");

    Stmt* initializer = nullptr;

    if (match({VAR})) {
        initializer = var_declaration();
    } else {
        initializer = expression_statement();
    }

    Expr* condition = nullptr;

    if (!check(SEMICOLON)) {
        condition = expression();
    }

    consume(SEMICOLON, "Expect ';' after loop condition.");

    Expr* increment = nullptr;
    if (!check(RIGHT_PAREN)) {
        increment = expression();
    }
    consume(RIGHT_PAREN, "Expect ')' after for clauses.");

    Stmt* body = statement();


    // Construct block stmt with initializer + desugared while-loop
    if (increment) {
        body = new BlockStmt({ body, new ExprStmt(increment) });
    }

    if (!condition) {
        condition = new BoolLiteral(true);
    }

    body = new WhileStmt(condition, body);

    if (initializer) {
        body = new BlockStmt({ initializer, body });
    }

    return body;
}

Stmt* Parser::func_statement(std::string kind) {
    Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
    consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");

    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 8) {
                error(peek(), "Cannot have more than 8 parameters.");
            }

            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
        } while (match({COMMA}));
    }

    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");

    std::vector<Stmt*> body;
    body.push_back(block_statement());

    return new FuncStmt(name, parameters, body);
}

Stmt* Parser::expression_statement() {
    const Expr* value = expression();
    consume(SEMICOLON, "Expect ';' after expression.");
    return new ExprStmt(value);
}


Expr* Parser::expression() {
    return assignment();
}


Expr* Parser::assignment() {
    // Don't have a lot of lookahead, so use a 'trick' here:
    // All assignment targets are valid exprs (ex - 'a.prop.b')
    // so parse LHS as an Expr and check that it's an l-value.
    Expr* expr = logical_or();

    if (match({EQUAL})) {
        Token equals = previous();
        Expr* value = assignment(); // right-associative, so recurse

        if (expr->lvalue()) {
            Variable* variable = static_cast<Variable*>(expr);
            return new Assignment(variable->name, *value);
        } else if (expr->is_object_field()) {
            Get* get = static_cast<Get*>(expr);
            return new Set(get->callee, get->name, *value);
        }

        Lox::error(equals, "Invalid assignment target.");
    }

    // If no assignment found, fall through to
    // the higher-precedence, valid Expr.
    return expr;
}

Expr* Parser::logical_or() {
    Expr* expr = logical_and();

    while (match({OR})) {
        Token op = previous();
        Expr* right = logical_and();
        expr = new Logical(*expr, op, *right);
    }

    return expr;
}

Expr* Parser::logical_and() {
    Expr* expr = equality();

    while (match({AND})) {
        Token op = previous();
        Expr* right = equality();
        expr = new Logical(*expr, op, *right);
    }

    return expr;
}

Expr* Parser::equality() {
    Expr* expr = comparison();

    while (match({BANG_EQUAL, EQUAL_EQUAL})) {
        Token op = previous();
        Expr* right = comparison();
        expr = (new Binary(*expr, op, *right));

    }

    return expr;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (const TokenType& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(const TokenType& type) {
    if (at_end()) return false;

    return peek().type == type;
}

Token& Parser::advance() {
    if (!at_end()) current++;
    return previous();
}

bool Parser::at_end() {
    return peek().type == END_OF_FILE;
}

/* Current (unconsumed) token */
Token& Parser::peek() {
    return tokens[current];
}

/* Most recently consumed token */
Token& Parser::previous() {
    return tokens[current - 1];
}

Expr* Parser::comparison() {
    Expr* expr = addition();

    while (match({ GREATER, GREATER_EQUAL, LESS, LESS_EQUAL })) {
        Token& op = previous();
        Expr* right = addition();
        expr = (new Binary(*expr, op, *right));
    }

    return expr;
}

Expr* Parser::addition() {
    Expr* expr = multiplication();

    while (match({ MINUS, PLUS })) {
        Token& op = previous();
        Expr* right = multiplication();
        expr = (new Binary(*expr, op, *right));
    }

    return expr;

}

Expr* Parser::multiplication() {
    Expr* expr = unary();

    while (match({ STAR, SLASH })) {
        Token& op = previous();
        Expr* right = unary();
        expr = (new Binary(*expr, op, *right));
    }

    return expr;
}

Expr* Parser::unary() {
    if (match({ BANG, MINUS })) {
        Token& op = previous();
        Expr* right = unary(); /* Unary is right-recursive */
        return (new Unary(op, *right));
    }

    return call();
}

Expr* Parser::call() {
    Expr* expr = primary();

    while (true) {
        if (match({LEFT_PAREN})) {
            /*
             * At this point, everything to left is the callee,
             * so as soon as '(' is seen, try to complete the call
             * expr (or sequence of call exprs). If there is a sequence,
             * all previous calls will become part of the (new) callee.
             *
             * ex - returns_fn()also_returns_fn(arg)returns_val();
             */
            expr = finish_call(expr);
        } else if (match({DOT})) {
            Token name = consume(IDENTIFIER,
                                 "Expect property name after '.'.");
            expr = new Get(*expr, name);
        }
        else {
            break;
        }
    }

    return expr;
}

Expr* Parser::finish_call(Expr* callee) {
    std::vector<Expr*> args;

    if (!check(RIGHT_PAREN)) {
        do {
            if (args.size() >= 8) {
                error(peek(), "Cannot have more than 8 arguments.");
            }
            args.push_back(expression());
        } while (match({COMMA}));
    }

    Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");

    return new Call(*callee, paren, args);
}

Expr* Parser::lambda_expr() {
    /*
     *  Lambda syntax:
     *  lambda (args) { return expr; }
     *  ex : list_to_double.map(lambda (x) { return x * 2; })
     * */

    consume(LEFT_PAREN, "Expect '(' after lambda keyword");

    std::vector<Token> parameters;
    if (!check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 8) {
                error(peek(), "Cannot have more than 8 parameters.");
            }

            parameters.push_back(consume(IDENTIFIER, "Expect parameter name."));
        } while (match({COMMA}));
    }

    consume(RIGHT_PAREN, "Expect ')' after parameters.");

    consume(LEFT_BRACE, "Expect '{' before lambda body.");

    std::vector<Stmt*> body;
    body.push_back(block_statement());

    return new Lambda(parameters, body);
}

Expr* Parser::primary() {
    if (match({LAMBDA}))        return lambda_expr();
    if (match({THIS}))          return new This(previous());
    if (match({FALSE}))         return new BoolLiteral(false);
    if (match({TRUE}))          return new BoolLiteral(true);
    if (match({NIL}))           return new StrLiteral("nil", true);
    if (match({NUMBER}))        return new NumLiteral(stod(previous().literal));
    if (match({STRING}))        return new StrLiteral(previous().literal);
    if (match({IDENTIFIER}))    return new Variable(previous());
    if (match({LEFT_PAREN})) {
        Expr* expr = expression();
        consume(RIGHT_PAREN, "Expect ')' after expression.");
        return (new Grouping(*expr));
    }
    // throw error(peek(), "Expect expression.");
    return nullptr;
}

/* Error handling and recovery */

Token& Parser::consume(TokenType type, std::string msg) {
    if (check(type)) return advance();
    // throw error(peek(), msg); /* Report error with current token *;
}

ParseErr Parser::error(Token token, std::string msg) {
    Lox::error(token, msg);
    return ParseErr(msg);

}
void Parser::synchronize() {
    advance();

    while (!at_end()) {
        switch (peek().type) {
            case CLASS:
            case FUN:
            case IF:
            case FOR:
            case WHILE:
            case VAR:
            case PRINT:
            case RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}