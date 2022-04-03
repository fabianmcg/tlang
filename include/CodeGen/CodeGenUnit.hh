#ifndef CODEGEN_CODEGENUNIT_HH
#define CODEGEN_CODEGENUNIT_HH

#include <llvm/Support/raw_ostream.h>
#include "CodeEmitter.hh"

namespace tlang::codegen {
class CodeGenUnit {
public:
  CodeGenUnit(CodeEmitter &emitter, UnitDecl &unit, ASTContext &ast_context, llvm::LLVMContext &llvm_context);
  void emit(llvm::raw_ostream &ost);
protected:
  void init();
  CodeEmitter &emitter;
  UnitDecl &unit;
  ASTContext &ast_context;
  llvm::LLVMContext &llvm_context;
  std::unique_ptr<llvm::IRBuilder<>> builder { };
  std::unique_ptr<llvm::Module> module { };
};
}

#endif
