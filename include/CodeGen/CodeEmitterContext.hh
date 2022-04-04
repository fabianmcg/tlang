#ifndef CODEGEN_CODEEMITTERCONTEXT_HH
#define CODEGEN_CODEEMITTERCONTEXT_HH

#include <AST/ASTContext.hh>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace tlang::codegen {
class CodeEmitterContext {
public:
  CodeEmitterContext(ASTContext &ast_context, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) :
      ast_context(ast_context), context(context), builder(builder), module(module) {
  }
  llvm::LLVMContext& operator*() {
    return context;
  }
  llvm::IRBuilder<>* operator->() {
    return &builder;
  }
protected:
  ASTContext &ast_context;
  llvm::LLVMContext &context;
  llvm::IRBuilder<> &builder;
  llvm::Module &module;
};
template <typename T, typename ...Args>
T makeEmitter(ASTContext &ast_context, llvm::LLVMContext &llvm_context, llvm::Module &module, llvm::IRBuilder<> &builder, Args &&...args) {
  return T { ast_context, llvm_context, builder, module, std::forward<Args>(args)... };
}
}

#endif
