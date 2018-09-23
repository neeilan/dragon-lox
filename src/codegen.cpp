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

void CodeGenerator::visit(const Unary*) {};
void CodeGenerator::visit(const Binary*) {};
void CodeGenerator::visit(const Grouping*) {};
void CodeGenerator::visit(const StrLiteral*) {};
void CodeGenerator::visit(const NumLiteral*) {};
void CodeGenerator::visit(const BoolLiteral*) {};
void CodeGenerator::visit(const Variable*) {};
void CodeGenerator::visit(const Assignment*) {};
void CodeGenerator::visit(const Logical*) {};
void CodeGenerator::visit(const Call*) {};
void CodeGenerator::visit(const Get*) {};
void CodeGenerator::visit(const Set*) {};
void CodeGenerator::visit(const This*) {};
void CodeGenerator::visit(const Lambda*) {};