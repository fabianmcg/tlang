#ifndef CODEGEN_CODEGEN_HH
#define CODEGEN_CODEGEN_HH

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <AST/ASTContext.hh>

namespace tlang::codegen {
class CodeGen {
public:
  CodeGen(ASTContext &ast_context) :
      ast_context(ast_context) {
    init();
  }
  llvm::Module* emit(UnitDecl *unit);
  inline std::map<std::string, std::unique_ptr<llvm::Module>>& getModules() {
    return modules;
  }
protected:
  void init();
  llvm::Module& getModule(UnitDecl *unit);
  ASTContext &ast_context;
  std::unique_ptr<llvm::LLVMContext> llvm_context { };
  std::map<std::string, std::unique_ptr<llvm::Module>> modules;
};
}
#endif
