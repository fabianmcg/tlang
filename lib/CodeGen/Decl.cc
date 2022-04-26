#include <AST/Visitors/ASTVisitor.hh>
#include <CodeGen/GenericEmitter.hh>
#include <Support/Enumerate.hh>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>

namespace tlang::codegen {
template <typename E>
struct VariableAllocator: ASTVisitor<VariableAllocator<E>, VisitorPattern::preOrder> {
  using visit_t = typename ASTVisitor<VariableAllocator<E>, VisitorPattern::preOrder>::visit_t;
  VariableAllocator(E &emitter) :
      emitter(emitter) {
  }
  visit_t visitVariableDecl(VariableDecl *node) {
    if (auto vd = dyn_cast<VariableDecl>(node))
      emitter.emitVariableDecl(node);
    return visit_t::skip;
  }
  visit_t visitTagDecl(TagDecl *node) {
    return visit_t::skip;
  }
  visit_t visitFunctorDecl(FunctorDecl *node) {
    return visit_t::skip;
  }
  visit_t visitExpr(Expr *node) {
    return visit_t::skip;
  }
  visit_t visitQualType(QualType *node) {
    return visit_t::skip;
  }
  E &emitter;
};
template <typename E>
struct FwdDeclarator: ASTVisitor<FwdDeclarator<E>, VisitorPattern::preOrder> {
  using visit_t = typename ASTVisitor<FwdDeclarator<E>, VisitorPattern::preOrder>::visit_t;
  FwdDeclarator(E &emitter) :
      emitter(emitter) {
  }
  visit_t visitFunctorDecl(FunctorDecl *node) {
    emitter.makeFunction(node);
    return visit_t::skip;
  }
  E &emitter;
};
void GenericEmitter::emitForwardDecl(UnitDecl *unit) {
  FwdDeclarator<GenericEmitter> { *this }.traverseUnitDecl(unit);
}
llvm::AllocaInst* GenericEmitter::makeVariable(VariableDecl *variable, const std::string &suffix) {
  llvm::Twine name = variable->getIdentifier() + suffix;
  llvm::AllocaInst *alloca = builder.CreateAlloca(emitQualType(variable->getType()), (unsigned) (variable->isShared() ? 3 : 0), nullptr,
      name);
  get(variable) = alloca;
  return alloca;
}
IRType_t<FunctionType> GenericEmitter::makeFunctionType(FunctorDecl *functor) {
  return emitFunctionType(dyn_cast<FunctionType>(functor->getType().getType()));
}
IRType_t<FunctorDecl> GenericEmitter::makeFunction(FunctorDecl *functor) {
  auto &irFunction = get(functor);
  if (irFunction)
    return static_cast<IRType_t<FunctorDecl>>(irFunction);
  llvm::FunctionType *functionType = makeFunctionType(functor);
  irFunction = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, functor->getIdentifier(), module);
  get(functor) = irFunction;
  return static_cast<IRType_t<FunctorDecl>>(irFunction);
}
IRType_t<ModuleDecl> GenericEmitter::emitModuleDecl(ModuleDecl *node) {
  IRType_t<ModuleDecl> last { };
  for (auto symbol : *static_cast<DeclContext*>(node))
    last = emitDecl(symbol);
  return last;
}
IRType_t<ExternFunctionDecl> GenericEmitter::emitExternFunctionDecl(ExternFunctionDecl *function) {
  llvm::Function *irFunction = makeFunction(function);
  return irFunction;
}
IRType_t<FunctionDecl> GenericEmitter::emitFunctionDecl(FunctionDecl *function) {
  llvm::Function *irFunction = makeFunction(function);
  llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context, "entry_block", irFunction);
  builder.SetInsertPoint(entry_block);
  auto &parameters = function->getParameters();
  for (auto &indexValue : enumerate(irFunction->args())) {
    indexValue.value().setName(parameters[indexValue.index()]->getIdentifier());
    get(parameters[indexValue.index()]) = &indexValue.value();
    emitParameterDecl(parameters[indexValue.index()]);
  }
  VariableAllocator<GenericEmitter> variableEmitter { *this };
  variableEmitter.traverseCompoundStmt(function->getBody());

  resetCounters();
  emitStmt(function->getBody());

  if (irFunction->getReturnType()->isVoidTy() && !builder.GetInsertBlock()->getTerminator())
    builder.CreateRetVoid();
  verifyFunction(*irFunction);
  if (function->isKernel()) {
    llvm::SmallVector<llvm::Metadata*, 32> Ops; // Tuple operands

    Ops.push_back(llvm::ValueAsMetadata::getConstant(irFunction));
    Ops.push_back(llvm::MDString::get(context, "kernel"));

    // get constant i32 1
    llvm::Type *I32Ty = llvm::Type::getInt32Ty(context);
    llvm::Constant *One = llvm::ConstantInt::get(I32Ty, 1);
    Ops.push_back(llvm::ValueAsMetadata::getConstant(One));

    auto *Node = llvm::MDTuple::get(context, Ops);
    llvm::NamedMDNode *nvvmMetadataNode = module.getOrInsertNamedMetadata("nvvm.annotations");
    nvvmMetadataNode->addOperand(Node);
  }
  return irFunction;
}
IRType_t<VariableDecl> GenericEmitter::emitVariableDecl(VariableDecl *variable) {
  if (variable->getStorage() == VariableDecl::Shared) {
    llvm::GlobalVariable *gv = module.getGlobalVariable(variable->getIdentifier());
    if (!gv)
    gv = new llvm::GlobalVariable(module, emitQualType(variable->getType()), false,
        llvm::GlobalVariable::InternalLinkage, nullptr, variable->getIdentifier(), nullptr,
        llvm::GlobalValue::ThreadLocalMode::NotThreadLocal, 3);
    get(variable) = gv;
    gv->setUnnamedAddr(llvm::GlobalVariable::UnnamedAddr::None);
    gv->setInitializer(llvm::Constant::getNullValue(emitQualType(variable->getType())));
    return gv;
  }
  return makeVariable(variable);
}
IRType_t<ParameterDecl> GenericEmitter::emitParameterDecl(ParameterDecl *variable) {
  if (variable->getIdentifier().empty())
    return nullptr;
  auto argument = get(variable);
  auto alloca = makeVariable(variable, "_param");
  builder.CreateStore(argument, alloca);
  return alloca;
}
}
