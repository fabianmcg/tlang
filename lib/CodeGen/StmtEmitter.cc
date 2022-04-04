#include <AST/Visitors/ASTVisitor.hh>
#include <CodeGen/StmtEmitter.hh>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>


namespace tlang::codegen {
StmtEmitter::StmtEmitter(Emitter &emitter, TypeEmitter &type_emitter, ExprEmitterVisitor &expr_emitter) :
    CodeEmitterContext(static_cast<CodeEmitterContext&>(emitter)), EmitterTable(emitter), typeEmitter(type_emitter), exprEmitter(
        expr_emitter) {
}
}
