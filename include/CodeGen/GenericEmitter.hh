#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <deque>
#include <CodeGen/CodeGenVisitor.hh>
#include <CodeGen/CodeEmitter.hh>

namespace tlang::codegen {
class GenericEmitter: public CodeEmitter, public CodeGenVisitor<GenericEmitter> {
public:
  using CodeEmitter::CodeEmitter;
  llvm::PointerType* makePointer(llvm::Type* type, unsigned adressSpace = 0);
  llvm::Type* makeVoid();
  return_t<QualType> emitQualType(QualType *type);
  return_t<AddressType> emitAddressType(AddressType *type);
  return_t<BoolType> emitBoolType(BoolType *type);
  return_t<IntType> emitIntType(IntType *type);
  return_t<FloatType> emitFloatType(FloatType *type);
  return_t<PtrType> emitPtrType(PtrType *type);
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
