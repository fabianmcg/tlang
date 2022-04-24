#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
TypeEmitter::TypeEmitter(llvm::LLVMContext &context) :
    context(context) {

}
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
  return makeAddress(addressSpace);
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
  auto as = addressSpace;
  addressSpace = 0;
  return makePointer(static_cast<llvm::Type*>(emitType(type->getUnderlying())), as);
}
IRType_t<PtrType> TypeEmitter::emitPtrType(PtrType *type) {
  auto as = addressSpace;
  addressSpace = 0;
  return makePointer(static_cast<llvm::Type*>(emitType(type->getUnderlying())), as);
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
IRType_t<FunctionType> TypeEmitter::emitFunctionType(FunctionType *type) {
  auto &parameters = type->getParemeters();
  std::vector<llvm::Type*> ir_params(parameters.size(), nullptr);
  for (size_t i = 0; i < parameters.size(); ++i)
    ir_params[i] = emitQualType(parameters[i]);
  auto ret_type = emitQualType(type->getReturnType());
  return llvm::FunctionType::get(ret_type, ir_params, false);
}
llvm::Type* TypeEmitter::emitQualType(QualType type) {
  auto canonical = type.getCanonicalType();
  auto canonicalType = canonical.getType();
  if (!canonicalType)
    return makeVoid();
  if (type.getAddressSpace())
    addressSpace = type.getAddressSpace();
  auto emitted = emitType(canonicalType);
  if (canonical.isReference())
    emitted = makePointer(static_cast<llvm::Type*>(emitted), addressSpace);
  addressSpace = 0;
  return emitted;
}
IRType_t<QualType> TypeEmitter::operator()(QualType &type) {
  return emitQualType(type);
}
} // namespace tlang::codegen
