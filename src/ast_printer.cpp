#include "ast_printer.hpp"
#include "expr.hpp"

std::string AstPrinter::visit(const Binary* expr) {

    return parenthesize((expr->op).lexeme, &(expr->left), &(expr->right));
}

std::string AstPrinter::visit(const StrLiteral* expr) {
    if (expr->nil) {
        return "nil";
    } else {
        return expr->value;
    }
}

std::string AstPrinter::visit(const NumLiteral* expr) {
    if (expr->nil) {
        return "nil";
    } else {
        return std::to_string(expr->value);
    }
}

std::string AstPrinter::visit(const BoolLiteral* expr) {
    if (expr->value) {
        return "TRUE";
    } else {
        return "FALSE";
    }
}

std::string AstPrinter::visit(const Grouping* expr) {
    return parenthesize(std::string("group"), &(expr->expression));
}

std::string AstPrinter::visit(const Unary* expr) {
    return parenthesize((expr->op).lexeme, &(expr->right));
}

std::string AstPrinter::visit(const Variable *expr) {
    return expr->name.lexeme;
}

std::string AstPrinter::visit(const Logical *expr) {
    return parenthesize(expr->op.lexeme, &expr->left, &expr->right);
}

std::string AstPrinter::visit(const Lambda *expr) {
    return "<anonymous lambda>";
}

std::string AstPrinter::parenthesize(std::string name, const Expr* expr) {
    return "(" + name + " " + expr->accept(this) + ")";
}

std::string AstPrinter::parenthesize(std::string name, const Expr* expr1, const Expr* expr2) {
    return "(" + name + " " + expr1->accept(this) + " " + expr2->accept(this) + ")";
}