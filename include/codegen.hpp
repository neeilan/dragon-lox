//
// Created by Neeilan Selvalingam on 9/23/18.
//

#ifndef LOXPP_CODEGEN_HPP
#define LOXPP_CODEGEN_HPP

#include <map>

#include "expr.hpp"
#include "visitor.h"

#include "llvm/IR/Value.h"

using llvm::Value;

class CodeGenerator : public ExprVisitor<Value*> {
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
    void resolve(const Expr* expr, int depth);
    void print();

private:
    std::map<const Expr*, int> locals;
    
};

#endif //LOXPP_CODEGEN_HPP
