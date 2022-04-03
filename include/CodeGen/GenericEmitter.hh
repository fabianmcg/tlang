#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <deque>
#include <CodeGen/CodeGenVisitor.hh>
#include <CodeGen/CodeEmitter.hh>

namespace tlang::codegen {
class GenericEmitter: public CodeEmitter, public CodeGenVisitor<GenericEmitter> {
public:
  using CodeEmitter::CodeEmitter;
  virtual ~GenericEmitter() = default;
  virtual void run(UnitDecl *unit);
protected:
  std::map<ASTNode*, llvm::Value*>& values();
  void pushContext();
  void popContext();
  std::map<Decl*, llvm::Type*> decl2type;
  std::deque<std::map<ASTNode*, llvm::Value*>> valueTable;
};
}

#endif
