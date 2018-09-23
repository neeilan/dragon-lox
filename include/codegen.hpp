//
// Created by Neeilan Selvalingam on 9/23/18.
//

#ifndef LOXPP_CODEGEN_HPP
#define LOXPP_CODEGEN_HPP

// DragonLox libraries
#include "expr.hpp"
#include "visitor.h"

class CodeGenerator : public ExprVisitor<void> {
public:
    virtual void visit(const Unary*);
    virtual void visit(const Binary*);
    virtual void visit(const Grouping*);
    virtual void visit(const StrLiteral*);
    virtual void visit(const NumLiteral*);
    virtual void visit(const BoolLiteral*);
    virtual void visit(const Variable*);
    virtual void visit(const Assignment*);
    virtual void visit(const Logical*);
    virtual void visit(const Call*);
    virtual void visit(const Get*);
    virtual void visit(const Set*);
    virtual void visit(const This*);
    virtual void visit(const Lambda*);
    
};

#endif //LOXPP_CODEGEN_HPP
