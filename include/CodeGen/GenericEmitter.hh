#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <CodeGen/Emitter.hh>
#include <CodeGen/TypeEmitter.hh>
#include <CodeGen/DeclEmitter.hh>

namespace tlang::codegen {
class GenericEmitter: public Emitter {
public:
  GenericEmitter(ASTContext &ast_context, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module);
  void run(UnitDecl *unit);
protected:
  void init();
  TypeEmitter typeEmitter;
  std::unique_ptr<DeclEmitter> declEmitter { };
};
}

#endif
