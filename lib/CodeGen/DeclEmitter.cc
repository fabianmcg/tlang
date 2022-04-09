#include <Support/Enumerate.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <CodeGen/DeclEmitter.hh>
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
DeclEmitter::DeclEmitter(Emitter &emitter, TypeEmitter &type_emitter, StmtEmitterVisitor &stmt_emitter) :
    CodeEmitterContext(static_cast<CodeEmitterContext&>(emitter)), EmitterTable(emitter), typeEmitter(type_emitter), stmtEmitter(
        stmt_emitter) {
}
llvm::AllocaInst* DeclEmitter::makeVariable(VariableDecl *variable, const std::string &suffix) {
  llvm::Twine name = variable->getIdentifier() + suffix;
  llvm::AllocaInst *alloca = builder.CreateAlloca(typeEmitter.emitQualType(variable->getType()), 0, name);
  get(variable) = alloca;
  return alloca;
}
IRType_t<FunctionType> DeclEmitter::makeFunctionType(FunctorDecl *functor) {
  return typeEmitter.emitFunctionType(dyn_cast<FunctionType>(functor->getType().getType()));
}
IRType_t<FunctorDecl> DeclEmitter::makeFunction(FunctorDecl *functor) {
  llvm::FunctionType *functionType = makeFunctionType(functor);
  llvm::Function *irFunction = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, functor->getIdentifier(), module);
  get(functor) = irFunction;
  return irFunction;
}
IRType_t<UnitDecl> DeclEmitterVisitor::emitUnitDecl(UnitDecl *node) {
  IRType_t<UnitDecl> last { };
  for (auto symbol : *static_cast<UnitContext*>(node))
    last = emitDecl(symbol);
  return last;
}
IRType_t<ModuleDecl> DeclEmitterVisitor::emitModuleDecl(ModuleDecl *node) {
  IRType_t<ModuleDecl> last { };
  for (auto symbol : *static_cast<DeclContext*>(node))
    last = emitDecl(symbol);
  return last;
}
IRType_t<FunctionDecl> DeclEmitterVisitor::emitFunctionDecl(FunctionDecl *function) {
  llvm::Function *irFunction = makeFunction(function);
  llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context, "entry_block", irFunction);
  builder.SetInsertPoint(entry_block);
  auto &parameters = function->getParameters();
  for (auto &indexValue : enumerate(irFunction->args())) {
    indexValue.value().setName(parameters[indexValue.index()]->getIdentifier());
    get(parameters[indexValue.index()]) = &indexValue.value();
    emitParameterDecl(parameters[indexValue.index()]);
  }
  VariableAllocator<DeclEmitterVisitor> variableEmitter { *this };
  variableEmitter.traverseCompoundStmt(function->getBody());
  stmtEmitter.resetCounters();
  stmtEmitter.emitStmt(function->getBody());
  if (irFunction->getReturnType()->isVoidTy() && !builder.GetInsertBlock()->getTerminator())
    builder.CreateRetVoid();
  verifyFunction(*irFunction);
  return irFunction;
}
IRType_t<VariableDecl> DeclEmitterVisitor::emitVariableDecl(VariableDecl *variable) {
  return makeVariable(variable);
}
IRType_t<ParameterDecl> DeclEmitterVisitor::emitParameterDecl(ParameterDecl *variable) {
  if (variable->getIdentifier().empty())
    return nullptr;
  auto argument = get(variable);
  auto alloca = makeVariable(variable, ".param");
  builder.CreateStore(argument, alloca);
  return alloca;
}
}
