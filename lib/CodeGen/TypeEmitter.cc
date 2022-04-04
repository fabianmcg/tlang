#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
llvm::Type* TypeEmitter::makeVoid() {
  return llvm::Type::getVoidTy(context);
}
llvm::PointerType* TypeEmitter::makeAddress(int adressSpace) {
  return llvm::Type::getInt8PtrTy(context, adressSpace < 0 ? 0 : static_cast<unsigned>(adressSpace));
}
llvm::PointerType* TypeEmitter::makePointer(llvm::Type *type, int adressSpace) {
  assert(type);
  return llvm::PointerType::get(type, adressSpace < 0 ? 0u : static_cast<unsigned>(adressSpace));
}
IRType_t<AddressType> TypeEmitter::emitAddressType(AddressType *type) {
  return makeAddress(adressSpace);
}
IRType_t<BoolType> TypeEmitter::emitBoolType(BoolType *type) {
  return llvm::Type::getInt1Ty(context);
}
IRType_t<IntType> TypeEmitter::emitIntType(IntType *type) {
  switch (type->getPrecision()) {
  case IntType::P_8:
    return llvm::Type::getInt8Ty(context);
  case IntType::P_16:
    return llvm::Type::getInt16Ty(context);
  case IntType::P_32:
    return llvm::Type::getInt32Ty(context);
  case IntType::P_64:
    return llvm::Type::getInt64Ty(context);
  default:
    return nullptr;
  }
}
IRType_t<FloatType> TypeEmitter::emitFloatType(FloatType *type) {
  switch (type->getPrecision()) {
  case FloatType::P_16:
    return llvm::Type::getHalfTy(context);
  case FloatType::P_32:
    return llvm::Type::getFloatTy(context);
  case FloatType::P_64:
    return llvm::Type::getDoubleTy(context);
  default:
    return nullptr;
  }
}
IRType_t<ArrayType> TypeEmitter::emitArrayType(ArrayType *type) {
  return makePointer(static_cast<llvm::Type*>(emitType(type->getUnderlying())), adressSpace);
}
IRType_t<PtrType> TypeEmitter::emitPtrType(PtrType *type) {
  return makePointer(static_cast<llvm::Type*>(emitType(type->getUnderlying())), adressSpace);
}
IRType_t<StructType> TypeEmitter::emitStructType(StructType *type) {
  if (auto decl = dyn_cast<StructDecl>(type->getDecl().data())) {
    auto &st = decl2type[decl];
    if (st)
      return llvm::dyn_cast<llvm::StructType>(st);
    auto emitted = llvm::StructType::create(context, decl->getIdentifier());
    auto &members = decl->getMembers();
    std::vector<llvm::Type*> body(members.size());
    int idx { };
    for (auto member : members)
      body[idx++] = emitQualType(member->getType());
    emitted->setBody(body);
    st = emitted;
    return emitted;
  }
  return nullptr;
}
llvm::Type* TypeEmitter::emitQualType(QualType &type) {
  auto canonical = type.getCanonicalType();
  auto canonicalType = canonical.getType();
  if (!canonicalType)
    return makeVoid();
  auto emitted = emitType(canonicalType);
  if (canonical.isReference())
    emitted = makePointer(static_cast<llvm::Type*>(emitted));
  return emitted;
}
IRType_t<QualType> TypeEmitter::operator()(QualType &type) {
  return emitQualType(type);
}
} // namespace tlang::codegen
