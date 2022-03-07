#ifndef __CODEGEN_CODEGEN_HH__
#define __CODEGEN_CODEGEN_HH__

#include "AST/Include.hh"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "Context.hh"
namespace tlang::codegen {
class CodeGen {
public:
  CodeGen(ASTContext &astCtx, const std::string &module_name) :
      astCtx(astCtx) {
    init(module_name);
  }
  void emit(ModuleDecl *m, std::ostream &ost);
protected:
  void init(const std::string &module_name);
  void generate(ModuleDecl *m);
  void print(std::ostream &ost);
  CGContext makeContext() {
    return CGContext { astCtx, *context, *builder, *module };
  }
  ASTContext &astCtx;
  std::unique_ptr<llvm::LLVMContext> context { };
  std::unique_ptr<llvm::IRBuilder<>> builder { };
  std::unique_ptr<llvm::Module> module { };
};
}
#endif
