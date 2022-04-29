#ifndef CODEGEN_CODEGEN_HH
#define CODEGEN_CODEGEN_HH

#include <filesystem>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <Frontend/CompilerInvocation.hh>

namespace tlang::codegen {
class CodeGen {
public:
  CodeGen(CompilerInvocation &CI) :
      compiler_context(CI) {
    init();
  }
  int emit(std::filesystem::path output);
  void emit(UnitDecl *unit, llvm::raw_ostream& ost);
  llvm::Module* emit(UnitDecl *unit);
  inline std::map<std::string, std::unique_ptr<llvm::Module>>& getModules() {
    return modules;
  }
protected:
  void init();
  llvm::Module& getModule(UnitDecl *unit);
  CompilerInvocation &compiler_context;
  std::unique_ptr<llvm::LLVMContext> llvm_context { };
  std::map<std::string, std::unique_ptr<llvm::Module>> modules;
};
}
#endif
