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
using llvm::BasicBlock;
using llvm::LLVMContext;
using llvm::IRBuilder;
using llvm::Value;
using llvm::Module;
using llvm::StringLiteral;
using llvm::Function;
using llvm::Type;
using llvm::FunctionType;

static LLVMContext llvm_context;
static IRBuilder<> ir_builder(llvm_context);
static std::unique_ptr<Module> module;
static std::map<std::string, Value *> named_values;

void CodeGenerator::resolve(const Expr* expr, int depth) {
    locals[expr] = depth;
}

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
        case SLASH: return ir_builder.CreateFDiv(lhs_value, rhs_value, "multmp");

        default: return nullptr;
    }
};

Value* CodeGenerator::visit(const Grouping* expr) {
    return expr->expression.accept(this);
};

Value* CodeGenerator::visit(const StrLiteral* expr) {
    // return ir_builder.CreateGlobalStringPtr(StringLiteral::);
};

Value* CodeGenerator::visit(const NumLiteral* expr) {
    return ConstantFP::get(llvm_context, APFloat(expr->value));
};

Value* CodeGenerator::visit(const BoolLiteral*) {
    return nullptr;
};

Value* CodeGenerator::visit(const Variable* expr) { return named_values[expr->name.lexeme]; };

Value* CodeGenerator::visit(const Assignment*) { return nullptr; };

Value* CodeGenerator::visit(const Logical*) { return nullptr; };

Value* CodeGenerator::visit(const Call* expr) {
    auto callee_name = expr->callee.accept(this)->getName();
    Function* callee = module->getFunction(callee_name);

    if (!callee) {
        // Error
        return nullptr;
    }

    if (callee->arg_size() != expr->args.size()) {
        // Error
    }

    std::vector<Value*> argv;
    for (unsigned long i = 0, e = expr->args.size(); i != e; i++)
        argv.push_back(expr->args[i]->accept(this));

    return ir_builder.CreateCall(callee, argv, "calltmp");
};

Value* CodeGenerator::visit(const Get*) { return nullptr; };

Value* CodeGenerator::visit(const Set*) { return nullptr; };

Value* CodeGenerator::visit(const This*) { return nullptr; };

Value* CodeGenerator::visit(const Lambda* expr) {
    // Only support doubles for now
    std::vector<Type*> doubles(expr->parameters.size(), Type::getDoubleTy(llvm_context));

    FunctionType* fn_type = FunctionType::get(Type::getDoubleTy(llvm_context), doubles, false);
    Function* lambda = Function::Create(fn_type, Function::ExternalLinkage, "anon_lambda", module.get());

    BasicBlock* bb = BasicBlock::Create(llvm_context, "lambda_entry", lambda);
    ir_builder.SetInsertPoint(bb);
    return nullptr;
};