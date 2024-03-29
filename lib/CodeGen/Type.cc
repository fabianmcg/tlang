#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/GenericEmitter.hh>

namespace tlang::codegen {
llvm::Type* GenericEmitter::makeVoid() {
  return llvm::Type::getVoidTy(context);
}
llvm::PointerType* GenericEmitter::makeAddress(int addressSpace) {
  return llvm::Type::getInt8PtrTy(context, addressSpace < 0 ? 0 : static_cast<unsigned>(addressSpace));
}
llvm::PointerType* GenericEmitter::makePointer(llvm::Type *type, int addressSpace) {
  assert(type);
  return llvm::PointerType::get(type, addressSpace < 0 ? 0u : static_cast<unsigned>(addressSpace));
}
IRType_t<AddressType> GenericEmitter::emitAddressType(AddressType *type, int addressSpace) {
  return makeAddress(addressSpace);
}
IRType_t<BoolType> GenericEmitter::emitBoolType(BoolType *type, int addressSpace) {
  return llvm::Type::getInt1Ty(context);
}
IRType_t<IntType> GenericEmitter::emitIntType(IntType *type, int addressSpace) {
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
IRType_t<FloatType> GenericEmitter::emitFloatType(FloatType *type, int addressSpace) {
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
IRType_t<ArrayType> GenericEmitter::emitArrayType(ArrayType *type, int addressSpace) {
  auto base = static_cast<llvm::Type*>(emitType(type->getUnderlying()));
  llvm::ArrayType *atype { };
  int i = 0;
  for (auto dim : type->getLayout()) {
    if (auto il = dyn_cast<IntegerLiteral>(dim))
      atype = llvm::ArrayType::get(i == 0 ? base : atype, il->getValue());
    else if (auto ul = dyn_cast<UIntegerLiteral>(dim))
      atype = llvm::ArrayType::get(i == 0 ? base : atype, ul->getValue());
    else
      assert(false);
    ++i;
  }
  assert(atype);
  return atype;
}
IRType_t<PtrType> GenericEmitter::emitPtrType(PtrType *type, int addressSpace) {
  return makePointer(static_cast<llvm::Type*>(emitType(type->getUnderlying())), addressSpace);
}
IRType_t<StructType> GenericEmitter::emitStructType(StructType *type, int addressSpace) {
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
IRType_t<FunctionType> GenericEmitter::emitFunctionType(FunctionType *type, int addressSpace) {
  auto &parameters = type->getParemeters();
  std::vector<llvm::Type*> ir_params(parameters.size(), nullptr);
  for (size_t i = 0; i < parameters.size(); ++i)
    ir_params[i] = emitQualType(parameters[i]);
  auto ret_type = emitQualType(type->getReturnType());
  return llvm::FunctionType::get(ret_type, ir_params, false);
}
llvm::Type* GenericEmitter::emitQualType(QualType type) {
  auto canonical = type.getCanonicalType();
  auto canonicalType = canonical.getType();
  if (!canonicalType)
    return makeVoid();
  auto emitted = emitType(canonicalType, type.getAddressSpace());
  if (canonical.isReference())
    emitted = makePointer(static_cast<llvm::Type*>(emitted), type.getAddressSpace());
  return emitted;
}
}
