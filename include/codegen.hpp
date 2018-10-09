//
// Created by Neeilan Selvalingam on 9/23/18.
//

#ifndef LOXPP_CODEGEN_HPP
#define LOXPP_CODEGEN_HPP

#include <map>
#include <memory>

#include "expr.hpp"
#include "visitor.h"
#include "environment.hpp"

#include "llvm/IR/Value.h"

using std::shared_ptr;
using llvm::Value;

class CodeGenerator : public ExprVisitor<Value*>, public StmtVisitor {
public:
    Value* visit(const Unary*);
    Value* visit(const Binary*);
    Value* visit(const Grouping*);
    Value* visit(const StrLiteral*);
    Value* visit(const NumLiteral*);
    Value* visit(const BoolLiteral*);
    Value* visit(const Variable*);
    Value* visit(const Assignment*);
    Value* visit(const Logical*);
    Value* visit(const Call*);
    Value* visit(const Get*);
    Value* visit(const Set*);
    Value* visit(const This*);
    Value* visit(const Lambda*);

    void visit(const BlockStmt*);
    void visit(const ExprStmt*);
    void visit(const PrintStmt*);
    void visit(const VarStmt*);
    void visit(const ClassStmt*);
    void visit(const IfStmt*);
    void visit(const WhileStmt*);
    void visit(const FuncStmt*);
    void visit(const ReturnStmt*);

    void resolve(const Expr* expr, int depth);
    void print(const Expr* expr);

private:
    Environment<shared_ptr<Value> > globals;
    Environment<shared_ptr<Value> >* environment = &globals;
    std::map<const Expr*, int> locals;
    
};

#endif //LOXPP_CODEGEN_HPP
