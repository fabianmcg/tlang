#ifndef CODEGEN_CODEGENCONTEXT_HH
#define CODEGEN_CODEGENCONTEXT_HH

#include <AST/ASTContext.hh>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "Context.hh"
namespace tlang::codegen {
class CodeGenContext {
public:
  CodeGenContext(){
    init();
  }
  llvm::Module* addModule(const std::string& name);
  void emit(ModuleDecl *m, std::ostream &ost);
protected:
  void init();
//  void generate(ModuleDecl *m);
  void print(std::ostream &ost);

  ASTContext &astCtx;
  std::unique_ptr<llvm::LLVMContext> context { };
  std::unique_ptr<llvm::IRBuilder<>> builder { };
  std::unique_ptr<llvm::Module> module { };
};
}
#endif
