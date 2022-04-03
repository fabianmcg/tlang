#include "CodeGen/Context.hh"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "Utility/Enumerate.hh"

namespace tlang::codegen {
template <typename E>
struct VariableAllocator: RecursiveASTVisitor<VariableAllocator<E>, VisitorPattern::preOrder, VisitReturn<VisitStatus>> {
  using parent_type = RecursiveASTVisitor<VariableAllocator<E>, VisitorPattern::preOrder, VisitReturn<VisitStatus>>;
  using visit_t = typename parent_type::visit_t;
  VariableAllocator(E &emitter) :
      emitter(emitter) {
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    if (node->isNot(NodeClass::ParameterDecl))
      emitter.emitVariableDecl(*node);
    return visit_t::skip;
  }
  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitExpr(Expr *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    return visit_t::skip;
  }
  E &emitter;
};
struct EmitDecl {
  EmitDecl(CGContext &context) :
      context(context) {
  }
  llvm::Function* emitExternFunctionDecl(ExternFunctionDecl &function) {
    auto &params = function.getParameters();
    std::vector<llvm::Type*> ir_params(params.size(), nullptr);
    for (size_t i = 0; i < params.size(); ++i)
      ir_params[i] = context.emitType(params[i]);
    auto ret_type = context.emitType(function.getReturntype());
    llvm::FunctionType *function_type = llvm::FunctionType::get(ret_type, ir_params, false);
    return llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, function.getIdentifier(), context.module);
  }
  llvm::Function* emitFunctorDecl(FunctorDecl &function) {
    using namespace llvm;
    auto &params = function.getParameters();
    std::vector<llvm::Type*> ir_params(params.size(), nullptr);
    for (size_t i = 0; i < params.size(); ++i)
      ir_params[i] = context.emitType(params[i]->getType());
    auto ret_type = context.emitType(function.getReturntype());
    llvm::FunctionType *function_type = llvm::FunctionType::get(ret_type, ir_params, false);
    Function *ir_function = Function::Create(function_type, Function::ExternalLinkage, function.getIdentifier(), context.module);
    context[&function] = ir_function;
    BasicBlock *entry_block = BasicBlock::Create(*context, "entry_block", ir_function);
    context.builder.SetInsertPoint(entry_block);
    unsigned index = 0;
    for (auto &argument : ir_function->args()) {
      argument.setName(params[index]->getIdentifier());
      context[params[index]] = &argument;
      emitParameterDecl(&argument, *params[index]);
      ++index;
    }
    VariableAllocator<EmitDecl> variableEmitter { *this };
    variableEmitter.traverseCompoundStmt(function.getBody());
    context.emitStmt(function.getBody());
    if (function_type->getReturnType()->isVoidTy() && !context.builder.GetInsertBlock()->getTerminator())
      context.builder.CreateRetVoid();
    verifyFunction(*ir_function);
    return ir_function;
  }
  llvm::AllocaInst* emitVariableDecl(VariableDecl &variable) {
    llvm::AllocaInst *alloca = context.builder.CreateAlloca(context.emitType(variable.getType()), 0, variable.getIdentifier().c_str());
    context[&variable] = alloca;
    return alloca;
  }
  llvm::AllocaInst* emitParameterDecl(llvm::Argument *argument, ParameterDecl &variable) {
    std::string id = variable.getIdentifier() + ".param";
    llvm::AllocaInst *alloca = context.builder.CreateAlloca(context.emitType(variable.getType()), 0, id.c_str());
    context.builder.CreateStore(argument, alloca);
    context[&variable] = alloca;
    return alloca;
  }
protected:
  CGContext &context;
};
llvm::Value* CGContext::emitDecl(Decl *decl) {
  if (auto varDecl = dynamic_cast<VariableDecl*>(decl)) {
    EmitDecl emitter { *this };
    return  emitter.emitVariableDecl(*varDecl);
  }
  return nullptr;
}
void CGContext::emitModuleDecl(ModuleDecl *module) {
  EmitDecl emitter { *this };
  for (auto &symbol : *module)
    if (auto decl = symbol.node) {
      if (auto function = dynamic_cast<FunctorDecl*>(decl))
        emitter.emitFunctorDecl(*function);
      else if (auto function = dynamic_cast<ExternFunctionDecl*>(decl))
        emitter.emitExternFunctionDecl(*function);
    }
}
}
