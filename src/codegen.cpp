//
// Created by Neeilan Selvalingam on 9/23/18.
//

#include <stmt.hpp>
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
using llvm::StringRef;
using llvm::PHINode;

static LLVMContext llvm_context;
static IRBuilder<> ir_builder(llvm_context);
static Module* module = new Module("Default module", llvm_context);
// std::unique_ptr<Module> module(StringRef("Default module"), llvm_context);
static std::map<std::string, Value *> named_values;


void CodeGenerator::generate(const std::vector<Stmt*> &stmts) {
    for (const Stmt* stmt : stmts) {
        stmt->accept(this);
    }
}

void CodeGenerator::resolve(const Expr* expr, int depth) {
    locals[expr] = depth;
}

void CodeGenerator::visit(const BlockStmt* stmt) {
    Environment<Value*>* previous = environment;

    environment = new Environment<Value*>(previous);


    for (const Stmt *inner_statement : stmt->block_contents)
    {
        inner_statement->accept(this);
    }

    delete environment;
    environment = previous;
}

void CodeGenerator::visit(const ExprStmt* stmt) {
    Value* val = stmt->expression->accept(this);
    val->print(llvm::outs());
}

void CodeGenerator::visit(const PrintStmt*) {

}

void CodeGenerator::visit(const VarStmt* stmt) {
    Value* value = stmt->expression->accept(this);
    environment->define(stmt->name.lexeme, value);
    value->print(llvm::outs());
}

void CodeGenerator::visit(const ClassStmt*) {

}

void CodeGenerator::visit(const IfStmt* stmt) {
    Value* condition = stmt->condition->accept(this);

    if (!condition) return;

    // Convert condition to a bool by comparing non-equal to 0.0.
    condition = ir_builder.CreateFCmpONE(condition, ConstantFP::get(llvm_context, APFloat(0.0)), "ifcond");

    Function *enclosing_func = ir_builder.GetInsertBlock()->getParent();

    // Create blocks for the then and else cases.  Insert the 'then' block at end of the function.
    BasicBlock *then_bb = BasicBlock::Create(llvm_context, "then", enclosing_func); // this constr. adds block to end of func
    BasicBlock *else_bb = BasicBlock::Create(llvm_context, "else");
    BasicBlock *merge_bb = BasicBlock::Create(llvm_context, "ifcont");

    ir_builder.CreateCondBr(condition, then_bb, else_bb);

    // Emit the then-block value
    ir_builder.SetInsertPoint(then_bb);
    stmt->then_branch->accept(this);
    ir_builder.CreateBr(merge_bb); // to ifcont - all BBs (incl. fall throughs) must be terminated in LLVM IR to pass verification
    then_bb = ir_builder.GetInsertBlock(); // for the phi node. Codegen-ing the then block can contain other BBs the incoming 'node' is emitted into

    // add the else-block to the function
    enclosing_func->getBasicBlockList().push_back(else_bb);

    // else-block value
    ir_builder.SetInsertPoint(else_bb);
    if (stmt->else_branch) {
        stmt->else_branch->accept(this);
    }

    ir_builder.CreateBr(merge_bb);

    enclosing_func->getBasicBlockList().push_back(merge_bb);
    ir_builder.SetInsertPoint(merge_bb);

}

void CodeGenerator::visit(const WhileStmt*) {}

void CodeGenerator::visit(const FuncStmt* stmt) {

    // Check if a func with this name exists
    Function *prev_func = module->getFunction(stmt->name.lexeme);
    assert(!prev_func && "Function with this name already exists");

    // For now, make func type (double...)
    std::vector<Type*> params(stmt->parameters.size(), Type::getDoubleTy(llvm_context));
    FunctionType* func_type = FunctionType::get(Type::getDoubleTy(llvm_context), params, false);
    Function *func = Function::Create(func_type, Function::ExternalLinkage, stmt->name.lexeme, module);

    // Set names for args (isn't strictly necessary)
    unsigned int idx = 0;
    for (auto &arg : func->args()) {
        arg.setName(stmt->parameters[idx++].lexeme);
    }

    BasicBlock* bb = BasicBlock::Create(llvm_context, "entry", func);
    ir_builder.SetInsertPoint(bb);

    for (auto &arg : func->args())
        environment->define(arg.getName(), &arg);

    // The function definition
    this->generate(stmt->body);

    func->print(llvm::outs());

    // Define the function globally
    environment->define(stmt->name.lexeme, func);

}

void CodeGenerator::visit(const ReturnStmt* stmt) {
    Value* retval = stmt->value->accept(this);

    if (retval)
        ir_builder.CreateRet(retval);
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

Value* CodeGenerator::visit(const Variable* expr) {
    Value* var = environment->get(expr->name);

//    if (!var) {
//        var = module->getFunction(expr->name.lexeme);
//    }

    assert(var && "Unknown variable name");

    return var;
}

Value* CodeGenerator::visit(const Assignment* expr) {
    environment->define(expr->name.lexeme, expr->value.accept(this));
};

Value* CodeGenerator::visit(const Logical*) { return nullptr; };

Value* CodeGenerator::visit(const Call* expr) {

    auto callee_val = expr->callee.accept(this);
    callee_val->print(llvm::outs());
    auto callee_name = callee_val->getName();

    Function* callee = module->getFunction(callee_name);

    if (!callee) {
        // Error
        assert (false && "Callee not found");
    }

    if (callee->arg_size() != expr->args.size()) {
        // Error
        assert (false && "Arg sizes don't match");
    }

    std::vector<Value*> argv;
    for (unsigned long i = 0, e = expr->args.size(); i != e; i++)
        argv.push_back(expr->args[i]->accept(this));

    return ir_builder.CreateCall(callee, argv, "calltmp");
};

Value* CodeGenerator::visit(const Lambda* expr) {
    // Only support doubles for now
    std::vector<Type*> doubles(expr->parameters.size(), Type::getDoubleTy(llvm_context));

    FunctionType* fn_type = FunctionType::get(Type::getDoubleTy(llvm_context), doubles, false);
    Function* lambda = Function::Create(fn_type, Function::ExternalLinkage, "anon_lambda", module);

    BasicBlock* bb = BasicBlock::Create(llvm_context, "lambda_entry", lambda);
    ir_builder.SetInsertPoint(bb);
    return nullptr;
};

Value* CodeGenerator::visit(const Get*) { return nullptr; };

Value* CodeGenerator::visit(const Set*) { return nullptr; };

Value* CodeGenerator::visit(const This*) { return nullptr; };
