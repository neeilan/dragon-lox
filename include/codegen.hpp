//
// Created by Neeilan Selvalingam on 9/23/18.
//

#ifndef LOXPP_CODEGEN_HPP
#define LOXPP_CODEGEN_HPP

#include "expr.hpp"
#include "visitor.h"

#include "llvm/IR/Value.h"

using llvm::Value;

class CodeGenerator : public ExprVisitor<Value*> {
public:
    virtual Value* visit(const Unary*);
    virtual Value* visit(const Binary*);
    virtual Value* visit(const Grouping*);
    virtual Value* visit(const StrLiteral*);
    virtual Value* visit(const NumLiteral*);
    virtual Value* visit(const BoolLiteral*);
    virtual Value* visit(const Variable*);
    virtual Value* visit(const Assignment*);
    virtual Value* visit(const Logical*);
    virtual Value* visit(const Call*);
    virtual Value* visit(const Get*);
    virtual Value* visit(const Set*);
    virtual Value* visit(const This*);
    virtual Value* visit(const Lambda*);
    void print();
    
};

#endif //LOXPP_CODEGEN_HPP
