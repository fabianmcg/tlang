#ifndef CODEGEN_CODEGENUNIT_HH
#define CODEGEN_CODEGENUNIT_HH

#include <llvm/Support/raw_ostream.h>
#include "CodeEmitter.hh"

namespace tlang::codegen {
class CodeGenUnit {
public:
  CodeGenUnit(UnitDecl *unit, ASTContext &ast_context, llvm::LLVMContext &llvm_context);
  void init();
  void emit(CodeEmitter &emitter, llvm::raw_ostream &ost);
  template <typename T, typename ...Args>
  T makeEmitter(Args &&...args) {
    return T { ast_context, llvm_context, *builder, *module, std::forward<Args>(args)... };
  }
protected:
  UnitDecl *unit;
  ASTContext &ast_context;
  llvm::LLVMContext &llvm_context;
  std::unique_ptr<llvm::IRBuilder<>> builder { };
  std::unique_ptr<llvm::Module> module { };
};
}

#endif
