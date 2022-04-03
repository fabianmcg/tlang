#include <CodeGen/CodeGen.hh>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace tlang::codegen {
using namespace _astnp_;
struct TypeVisitor: RecursiveASTVisitor<TypeVisitor, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  TypeVisitor(CGContext &context) :
      context(context) {
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    if (node->isReference())
      type = llvm::PointerType::get(type, 0);
    return visit_value;
  }
  visit_t visitPtrType(PtrType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::PointerType::get(type, 0);
    return visit_value;
  }
  visit_t visitIntType(IntType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::Type::getInt32Ty(context.context);
    return visit_value;
  }
  visit_t visitFloatType(FloatType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::Type::getDoubleTy(context.context);
    return visit_value;
  }
  visit_t visitStructType(StructType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::StructType::create(context.context, node->getIdentifier());
    context.decl2type[node->getDecl()] = type;
    return visit_value;
  }
  llvm::Type* emit(QualType &node) {
    if (!node.getType())
      return llvm::Type::getVoidTy(*context);
    traverseQualType(&node);
    return type;
  }
  llvm::Type* emit(Type *node) {
    traverseType(node);
    return type;
  }
  CGContext &context;
  llvm::Type *type { };
};
llvm::Type* CGContext::emitType(QualType &type) {
  return TypeVisitor { *this }.emit(type);
}
llvm::Type* CGContext::emitType(Type *type) {
  return TypeVisitor { *this }.emit(type);
}
}
