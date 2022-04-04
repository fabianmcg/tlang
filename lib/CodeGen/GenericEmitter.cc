#include <CodeGen/GenericEmitter.hh>

namespace tlang::codegen {
GenericEmitter::GenericEmitter(ASTContext &ast_context, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) :
    Emitter(ast_context, context, builder, module), typeEmitter(context) {
}
void GenericEmitter::init() {
  if (!exprEmitter)
    exprEmitter = std::make_unique<ExprEmitterVisitor>(*this, typeEmitter);
  if (!stmtEmitter)
    stmtEmitter = std::make_unique<StmtEmitterVisitor>(*this, typeEmitter, *exprEmitter);
  if (!declEmitter)
    declEmitter = std::make_unique<DeclEmitterVisitor>(*this, typeEmitter, *stmtEmitter);
}
void GenericEmitter::run(UnitDecl *unit) {
  init();
  if (declEmitter)
    declEmitter->emitUnitDecl(unit);
}
}
