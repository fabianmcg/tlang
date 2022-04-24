#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <CodeGen/Emitter.hh>
#include <CodeGen/DeclEmitter.hh>
#include <CodeGen/ExprEmitter.hh>
#include <CodeGen/StmtEmitter.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
class GenericEmitter: public Emitter {
public:
  GenericEmitter(ASTContext &ast_context, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module);
  void run(UnitDecl *unit);
protected:
  void init();
  TypeEmitter typeEmitter;
  std::unique_ptr<ExprEmitterVisitor> exprEmitter { };
  std::unique_ptr<StmtEmitterVisitor> stmtEmitter { };
  std::unique_ptr<DeclEmitterVisitor> declEmitter { };
};
}

#endif
