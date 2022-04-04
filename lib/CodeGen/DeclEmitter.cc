#include <CodeGen/DeclEmitter.hh>

namespace tlang::codegen {
DeclEmitter::DeclEmitter(Emitter &emitter, TypeEmitter &type_emitter) :
    CodeEmitterContext(static_cast<CodeEmitterContext&>(emitter)), EmitterTable(emitter), typeEmitter(type_emitter) {
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
IRType_t<UnitDecl> DeclEmitter::emitUnitDecl(UnitDecl *node) {
  IRType_t<UnitDecl> last { };
  for (auto &symbol : *static_cast<UnitContext*>(node))
    last = emitDecl(*symbol);
  return last;
}
IRType_t<ModuleDecl> DeclEmitter::emitModuleDecl(ModuleDecl *node) {
  IRType_t<ModuleDecl> last { };
  for (auto &symbol : *static_cast<DeclContext*>(node))
    last = emitDecl(*symbol);
  return last;
}
IRType_t<FunctionDecl> DeclEmitter::emitFunctionDecl(FunctionDecl *function) {
  llvm::Function *irFunction = makeFunction(function);
  llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context, "entry_block", irFunction);
  builder.SetInsertPoint(entry_block);
  auto &parameters = function->getParameters();
  unsigned index = 0;
  for (auto &argument : irFunction->args()) {
    argument.setName(parameters[index]->getIdentifier());
    get(parameters[index]) = &argument;
//    emitParameterDecl(&argument, *params[index]);
    ++index;
  }
//  VariableAllocator<EmitDecl> variableEmitter { *this };
//  variableEmitter.traverseCompoundStmt(function.getBody());
//  context.emitStmt(function.getBody());
//  if (function_type->getReturnType()->isVoidTy() && !context.builder.GetInsertBlock()->getTerminator())
//    context.builder.CreateRetVoid();
//  verifyFunction(*ir_function);
  return irFunction;
}
}
