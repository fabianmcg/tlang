#ifndef __CODEGEN_CONTEXT_HH__
#define __CODEGEN_CONTEXT_HH__

#include "AST/Include.hh"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"

namespace tlang::codegen {
struct CGContext {
  CGContext(ASTContext &astCtx, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) :
      astCtx(astCtx), context(context), builder(builder), module(module) {
  }
  inline llvm::LLVMContext& operator*() {
    return context;
  }
  inline llvm::Value*& operator[](ASTNode *node) {
    return valueTable[node];
  }
  llvm::Type* emitType(QualType &type);
  llvm::Type* emitType(Type *type);
//  llvm::Value* emitStmt(Stmt *stmt);
  llvm::Value* emitExpr(Expr *expr);
  ASTContext &astCtx;
  llvm::LLVMContext &context;
  llvm::IRBuilder<> &builder;
  llvm::Module &module;
  std::map<Decl*, llvm::Type*> decl2type;
  std::map<ASTNode*, llvm::Value*> valueTable;
};
}
#endif
