//
// Created by Neeilan Selvalingam on 9/23/18.
//

#include "codegen.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

using llvm::APFloat;
using llvm::ConstantFP;
using llvm::LLVMContext;
using llvm::IRBuilder;
using llvm::Value;
using llvm::Module;

static LLVMContext llvm_context;
static IRBuilder<> ir_builder(llvm_context);
static std::unique_ptr<Module> module;
static std::map<std::string, Value *> named_values;

void CodeGenerator::print() {

}

Value* CodeGenerator::visit(const Unary* expr) {
    Value* minus_one = ConstantFP::get(llvm_context, APFloat(-1.0));
    Value* rhs_value = expr->right.accept(this);
    return ir_builder.CreateFMul(minus_one, rhs_value, "subtmp");
};


Value* CodeGenerator::visit(const Binary* expr) {
    Value* lhs_value = expr->left.accept(this);
    Value* rhs_value = expr->right.accept(this);

    switch(expr->op.type) {
        case PLUS: return ir_builder.CreateFAdd(lhs_value, rhs_value, "addtmp");
        case MINUS: return ir_builder.CreateFSub(lhs_value, rhs_value, "subtmp");
        case STAR: return ir_builder.CreateFMul(lhs_value, rhs_value, "multmp");
        default: return nullptr;
    }
};

Value* CodeGenerator::visit(const Grouping*) { return nullptr; };
Value* CodeGenerator::visit(const StrLiteral*) { return nullptr; };

Value* CodeGenerator::visit(const NumLiteral* expr) {
    return ConstantFP::get(llvm_context, APFloat(expr->value));
};

Value* CodeGenerator::visit(const BoolLiteral*) { return nullptr; };
Value* CodeGenerator::visit(const Variable*) { return nullptr; };
Value* CodeGenerator::visit(const Assignment*) { return nullptr; };
Value* CodeGenerator::visit(const Logical*) { return nullptr; };

Value* CodeGenerator::visit(const Call*) { return nullptr; };

Value* CodeGenerator::visit(const Get*) { return nullptr; };
Value* CodeGenerator::visit(const Set*) { return nullptr; };
Value* CodeGenerator::visit(const This*) { return nullptr; };
Value* CodeGenerator::visit(const Lambda*) { return nullptr; };