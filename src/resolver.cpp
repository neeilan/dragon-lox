#include <string>

//#include "lox.hpp"
#include "resolver.hpp"
#include "stmt.hpp"

void Resolver::resolve(const std::vector<Stmt *> statements) {
    for (const Stmt* stmt : statements) {
        resolve(stmt);
    }
}

void Resolver::resolve(const Stmt *stmt) {
    stmt->accept(this);
}

void Resolver::resolve(const Expr *expr) {
    expr->accept(this);
}

void Resolver::visit(const BlockStmt* stmt) {
    begin_scope();
    resolve(stmt->block_contents);
    end_scope();
}

void Resolver::visit(const VarStmt *stmt) {
    declare(stmt->name);
    if (stmt->expression) {
        resolve(stmt->expression);
    }
    define(stmt->name);
}

void Resolver::visit(const Variable *expr) {
    if (!scopes.empty()
        && scopes.back()->count(expr->name.lexeme) > 0
        && !scopes.back()->at(expr->name.lexeme)) {
//        Lox::error(expr->name,
//                  "Cannot read local variable in its own initializer.");
    }

    resolve_local(expr, expr->name);
}

void Resolver::visit(const Assignment *expr) {
    resolve(&expr->value); // resolve RHS, in case it references other vars
    resolve_local(expr, expr->name); // resolve the var being assigned to
}

void Resolver::visit(const This *expr) {
    if (current_class != ClassType::IN_CLASS) {
//        Lox::error(expr->keyword, "Cannot use 'this' outside of a class.");
        return;
    }

    resolve_local(expr, expr->keyword);
}

void Resolver::visit(const FuncStmt *stmt) {
    declare(stmt->name);
    define(stmt->name);
    resolve_fn(FunctionType::FUNCTION, stmt);
}

void Resolver::visit(const ClassStmt* stmt) {
    ClassType enclosing_class = current_class;
    current_class = ClassType::IN_CLASS;

    declare(stmt->name);
    define(stmt->name);

    if (stmt->superclass) {
        resolve(stmt->superclass);
    }

    begin_scope();

    /*
     * Whenever 'this' is encountered in a method, it will resolve to a
     * "local variable” in an implicit scope just outside the method body.
     */
    scopes.back()->insert(std::pair<std::string, bool>("this", true));

    for (const Stmt* method : stmt->methods) {
        FunctionType declaration = METHOD;

        const FuncStmt* method_fn = static_cast<const FuncStmt*>(method);

        if (method_fn->name.lexeme == "init") {
            declaration = INITIALIZER;
        }

        resolve_fn(declaration, method_fn);
    }

    end_scope();

    current_class = enclosing_class;
}

void Resolver::resolve_local(const Expr *expr, const Token name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i]->count(name.lexeme) > 0) {
            /*
             * Tell CodeGenerator the # of scopes between current scope and the scope
             * where the variable should resolve. If the variable was found in the
             * current scope, pass in 0. In the immediately enclosing scope, 1
             */
            code_generator.resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }

    // Not found. Assume it is global.

}

void Resolver::resolve_fn(FunctionType declaration, const FuncStmt *fn) {
    FunctionType enclosing_function = current_function;
    current_function = declaration;

    begin_scope();

    for (const Token& param : fn->parameters) {
        declare(param);
        define(param);
    }

    resolve(fn->body);
    end_scope();

    current_function = enclosing_function;
}

void Resolver::visit(const Lambda *expr) {
    auto fn = new FuncStmt(
            Token(LAMBDA, "lambda", "", 0),
            expr->parameters,
            expr->body);

    resolve_fn(FUNCTION, fn);
    delete fn;
}

void Resolver::begin_scope() {
    scopes.push_back(new std::map<std::string, bool>());
}

void Resolver::end_scope() {
    delete scopes.back();
    scopes.pop_back();
}

void Resolver::define(const Token name) {
    if (scopes.empty()) return;

    // mark as initialized & available for use
    std::map<std::string, bool>* scope = scopes.back();

    if (scope->count(name.lexeme) > 0) {
        scope->erase(name.lexeme);
    }

    scope->insert(std::pair<std::string, bool>(name.lexeme, true));
}

void Resolver::declare(const Token name) {
    if (scopes.empty()) return;

    if (scopes.back()->count(name.lexeme) > 0) {
//        Lox::error(name, "Variable with this name already declared in this scope.");
    }

    // mark as declared but uninitialized for use
    std::map<std::string, bool>* scope = scopes.back();
    scope->insert(std::pair<std::string, bool>(name.lexeme, false));
}

// Other syntax tree nodes

// Statements
void Resolver::visit(const ExprStmt* stmt) {
    resolve(stmt->expression);
}

void Resolver::visit(const IfStmt* stmt) {
    resolve(stmt->condition);

    resolve(stmt->then_branch);

    if (stmt->else_branch)
        resolve(stmt->else_branch);
}

void Resolver::visit(const PrintStmt* stmt) {
    resolve(stmt->expression);
}

void Resolver::visit(const ReturnStmt* stmt) {

    if (current_function == NOT_IN_FN) {
//        Lox::error(stmt->keyword, "Cannot return from top-level code.");
    }

    if (current_function == INITIALIZER && stmt->value) {
//        Lox::error(stmt->keyword, "Cannot return a value from initializer.");
    }

    if (stmt->value) {
        resolve(stmt->value);
    }

}

void Resolver::visit(const WhileStmt* stmt) {
    resolve(stmt->condition);
    resolve(stmt->body);
}

// Expressions

void Resolver::visit(const Binary* expr) {
    resolve(&expr->left);
    resolve(&expr->right);
}

void Resolver::visit(const Call *expr) {
    resolve(&expr->callee);

    for (const Expr* arg : expr->args) {
        resolve(arg);
    }
}

void Resolver::visit(const Get *expr) {
    resolve(&expr->callee);
}

void Resolver::visit(const Set *expr) {
    resolve(&expr->value);
    resolve(&expr->callee);
}

void Resolver::visit(const Grouping *expr) {
    resolve(&expr->expression);
}

void Resolver::visit(const Logical *expr) {
    resolve(&expr->left);
    resolve(&expr->right);
}

void Resolver::visit(const Unary *expr) {
    resolve(&expr->right);
}

// since literals contain no variables, nothing to be done
void Resolver::visit(const BoolLiteral *expr) {}
void Resolver::visit(const NumLiteral *expr) {}
void Resolver::visit(const StrLiteral *expr) {}

