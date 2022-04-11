#ifndef CODEGEN_TYPEEMITTER_HH
#define CODEGEN_TYPEEMITTER_HH

#include <unordered_map>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <AST/Type.hh>
#include <CodeGen/Traits.hh>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class TypeEmitter {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  IRType_t<Type> emitType(Type *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
};
} // namespace impl
class TypeEmitter: public impl::TypeEmitter<TypeEmitter> {
public:
  TypeEmitter(llvm::LLVMContext &context);
  llvm::Type* makeVoid();
  llvm::PointerType* makeAddress(int adressSpace = 0);
  llvm::PointerType* makePointer(llvm::Type *type, int adressSpace = 0);
  IRType_t<QualType> emitQualType(QualType *type);
  IRType_t<AddressType> emitAddressType(AddressType *type);
  IRType_t<BoolType> emitBoolType(BoolType *type);
  IRType_t<IntType> emitIntType(IntType *type);
  IRType_t<FloatType> emitFloatType(FloatType *type);
  IRType_t<PtrType> emitPtrType(PtrType *type);
  IRType_t<ArrayType> emitArrayType(ArrayType *type);
  IRType_t<StructType> emitStructType(StructType *type);
  IRType_t<FunctionType> emitFunctionType(FunctionType *type);
  IRType_t<QualType> emitQualType(QualType type);
  IRType_t<QualType> operator()(QualType &type);
protected:
  std::unordered_map<Decl*, llvm::Type*> decl2type { };
  llvm::LLVMContext &context;
  int addressSpace { };
};
} // namespace tlang::codegen

#endif
