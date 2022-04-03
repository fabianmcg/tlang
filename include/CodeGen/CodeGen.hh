#ifndef CODEGEN_CODEGEN_HH
#define CODEGEN_CODEGEN_HH

#include "CodeGenUnit.hh"

namespace tlang::codegen {
class CodeGen {
public:
  CodeGen(ASTContext &ast_context) :
      ast_context(ast_context) {
    init();
  }
  void emit(UnitDecl *unit, llvm::raw_ostream &ost);
protected:
  void init();
  void initEmitters();
  CodeGenUnit makeUnit(UnitDecl *unit);
  ASTContext &ast_context;
  std::unique_ptr<llvm::LLVMContext> llvm_context { };
  std::vector<std::unique_ptr<CodeEmitter>> emitters;
};
}
#endif
