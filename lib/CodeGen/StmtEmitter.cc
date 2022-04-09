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
IRType_t<IfStmt> StmtEmitterVisitor::emitIfStmt(IfStmt *stmt) {
  llvm::Function *function = builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *then_block = llvm::BasicBlock::Create(context, makeLabel(ASTKind::IfStmt, "IfStmt"), function);
  llvm::BasicBlock *else_block =
      stmt->getElseStmt() ? else_block = llvm::BasicBlock::Create(context, makeLabel(ASTKind::IfStmt, "ElseStmt")) : nullptr;
  llvm::BasicBlock *endIf_block = llvm::BasicBlock::Create(context, makeLabel(ASTKind::IfStmt, "EndIfStmt"));
  incrementLabel(ASTKind::IfStmt);

  // Emit condition
  auto conditionExpr = emitExpr(stmt->getCondition());
  auto condition = builder.CreateICmpNE(conditionExpr, llvm::ConstantInt::get(conditionExpr->getType(), 0));
  if (else_block)
    builder.CreateCondBr(condition, then_block, else_block);
  else
    builder.CreateCondBr(condition, then_block, endIf_block);

  // Emit ThenStmt
  builder.SetInsertPoint(then_block);
  emitStmt(stmt->getThenStmt());
  emitBranch(then_block, endIf_block);

  // Emit ElseStmt
  if (else_block) {
    function->getBasicBlockList().push_back(else_block);
    builder.SetInsertPoint(else_block);
    emitStmt(stmt->getThenStmt());
    emitBranch(else_block, endIf_block);
  }

  // Emit EndIf
  function->getBasicBlockList().push_back(endIf_block);
  builder.SetInsertPoint(endIf_block);
  return endIf_block;
}
IRType_t<ForStmt> StmtEmitterVisitor::emitForStmt(ForStmt *stmt) {
  llvm::Function *function = builder.GetInsertBlock()->getParent();
  for (auto &range_stmt : stmt->getRanges()) {
    VariableDecl *for_variable =
        range_stmt->getVariable() ? range_stmt->getVariable()->getDecl().getAs<VariableDecl>() : range_stmt->getDecl();
    auto range_expr = range_stmt->getRange();
    llvm::BasicBlock *for_preamble = llvm::BasicBlock::Create(context, makeLabel(ASTKind::ForStmt, "ForStmt"), function);
    llvm::BasicBlock *for_body = llvm::BasicBlock::Create(context, makeLabel(ASTKind::ForStmt, "ForBody"), function);
    llvm::BasicBlock *iteration_block = llvm::BasicBlock::Create(context, makeLabel(ASTKind::ForStmt, "ForIt"), function);
    llvm::BasicBlock *end_for = llvm::BasicBlock::Create(context, makeLabel(ASTKind::ForStmt, "EndForStmt"));
    incrementLabel(ASTKind::ForStmt);
    llvm::BasicBlock *tmp_block = end_for;
    std::swap(tmp_block, end_block);

    // Emit preamble
    emitBranch(for_preamble);
    llvm::AllocaInst *alloca = static_cast<llvm::AllocaInst*>(get(for_variable));
    builder.SetInsertPoint(for_preamble);
    auto start_value = emitExpr(range_expr->getStart());
    builder.CreateStore(start_value, alloca);
    llvm::Value *end_condition = emitStmt(range_expr->getStop());
    llvm::Value *tmp = builder.CreateICmpSLT(start_value, end_condition);
    builder.CreateCondBr(tmp, for_body, end_for);

    // Emit For body
    builder.SetInsertPoint(for_body);
    emitStmt(stmt->getBody());
    emitBranch(iteration_block);

    // Setup next iteration
    builder.SetInsertPoint(iteration_block);
    llvm::Value *step_value;
    if (range_expr->getStep())
      step_value = emitExpr(range_expr->getStep());
    else
      step_value = llvm::ConstantInt::get(emitQualType(range_expr->getStart()->getType()), 1);
    llvm::Value *it_value = builder.CreateLoad(alloca->getAllocatedType(), alloca);
    llvm::Value *nit_value = builder.CreateAdd(it_value, step_value);
    builder.CreateStore(nit_value, alloca);
    end_condition = builder.CreateICmpSLT(nit_value, end_condition);
    builder.CreateCondBr(end_condition, for_body, end_for);

    // Emit EndFor
    function->getBasicBlockList().push_back(end_for);
    builder.SetInsertPoint(end_for);
    std::swap(tmp_block, end_block);
  }
  return nullptr;
}
IRType_t<WhileStmt> StmtEmitterVisitor::emitWhileStmt(WhileStmt *stmt) {
  return nullptr;
}
IRType_t<BreakStmt> StmtEmitterVisitor::emitBreakStmt(BreakStmt *stmt) {
  return nullptr;
}
IRType_t<ContinueStmt> StmtEmitterVisitor::emitContinueStmt(ContinueStmt *stmt) {
  return nullptr;
}
IRType_t<ReturnStmt> StmtEmitterVisitor::emitReturnStmt(ReturnStmt *stmt) {
  if (!stmt->getReturn())
    builder.CreateRetVoid();
  auto ret = emitExpr(stmt->getReturn());
  return builder.CreateRet(ret);
}
}
