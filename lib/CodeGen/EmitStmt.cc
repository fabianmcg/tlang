#include "CodeGen/Context.hh"
#include "llvm/IR/Constants.h"

namespace tlang::codegen {

struct EmitStmt {
  EmitStmt(CGContext &context) :
      context(context) {
  }
  std::string makeLabel(NodeClass kind, const std::string label) {
    return label + "." + std::to_string(counters[kind]);
  }
  void incrementLabel(NodeClass kind) {
    counters[kind]++;
  }
  inline llvm::Value* emitBranch(llvm::BasicBlock *current_block, llvm::BasicBlock *to_block) {
    if (current_block && !current_block->getTerminator())
      return context.builder.CreateBr(to_block);
    return nullptr;
  }
  inline llvm::Value* emitBranch(llvm::BasicBlock *to_block) {
    return emitBranch(context.builder.GetInsertBlock(), to_block);
  }
  llvm::Value* emitBreakStmt(BreakStmt &expr) {
    return emitBranch(end_block);
  }
  llvm::Value* emitForStmt(ForStmt &stmt) {
    llvm::Function *function = context.builder.GetInsertBlock()->getParent();
    for (auto &range_stmt : stmt.getRange()) {
      VariableDecl *for_variable =
          range_stmt->getVariable() ? range_stmt->getVariable()->getDecl().getAs<VariableDecl>() : range_stmt->getDecl();
      auto range_expr = range_stmt->getRange();
      llvm::BasicBlock *for_preamble = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::ForStmt, "ForStmt"), function);
      llvm::BasicBlock *for_body = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::ForStmt, "ForBody"), function);
      llvm::BasicBlock *iteration_block = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::ForStmt, "ForIt"), function);
      llvm::BasicBlock *end_for = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::ForStmt, "EndForStmt"));
      incrementLabel(NodeClass::ForStmt);
      llvm::BasicBlock *tmp_block = end_for;
      std::swap(tmp_block, end_block);
      // Emit preamble
      emitBranch(for_preamble);
      llvm::AllocaInst *alloca = static_cast<llvm::AllocaInst*>(context[for_variable]);
      context.builder.SetInsertPoint(for_preamble);
      emitStmt(range_expr->getStart());
      auto start_value = emitStmt(range_expr->getStart());
      context.builder.CreateStore(start_value, alloca);
      llvm::Value *end_condition = emitStmt(range_expr->getStop());

      // Emit For body
      context.builder.CreateBr(for_body);
      context.builder.SetInsertPoint(for_body);
      emitStmt(stmt.getBody());
      context.builder.CreateBr(iteration_block);

      // Setup next iteration
      context.builder.SetInsertPoint(iteration_block);
      llvm::Value *step_value = step_value = llvm::ConstantInt::get(context.emitType(range_expr->getStart()->getType()), 1);
      llvm::Value *it_value = context.builder.CreateLoad(alloca->getAllocatedType(), alloca);
      llvm::Value *nit_value = context.builder.CreateAdd(it_value, step_value);
      context.builder.CreateStore(nit_value, alloca);
      end_condition = context.builder.CreateICmpSLT(nit_value, end_condition);
      context.builder.CreateCondBr(end_condition, for_body, end_for);

      // Emit EndFor
      function->getBasicBlockList().push_back(end_for);
      context.builder.SetInsertPoint(end_for);
      std::swap(tmp_block, end_block);
    }
    return nullptr;
  }
  llvm::Value* emitIfStmt(IfStmt &stmt) {
    llvm::Function *function = context.builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::IfStmt, "IfStmt"), function);
    llvm::BasicBlock *else_block =
        stmt.getElse() ? else_block = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::IfStmt, "ElseStmt")) : nullptr;
    llvm::BasicBlock *endIf_block = llvm::BasicBlock::Create(*context, makeLabel(NodeClass::IfStmt, "EndIfStmt"));
    incrementLabel(NodeClass::IfStmt);

    // Emit condition
    auto conditionExpr = emitExpr(*stmt.getCondition());
    auto condition = context.builder.CreateICmpNE(conditionExpr, llvm::ConstantInt::get(conditionExpr->getType(), 0));
    if (else_block)
      context.builder.CreateCondBr(condition, then_block, else_block);
    else
      context.builder.CreateCondBr(condition, then_block, endIf_block);

    // Emit ThenStmt
    context.builder.SetInsertPoint(then_block);
    emitStmt(stmt.getThen());
    emitBranch(then_block, endIf_block);

    // Emit ElseStmt
    if (else_block) {
      function->getBasicBlockList().push_back(else_block);
      context.builder.SetInsertPoint(else_block);
      emitStmt(stmt.getThen());
      emitBranch(else_block, endIf_block);
    }

    // Emit EndIf
    function->getBasicBlockList().push_back(endIf_block);
    context.builder.SetInsertPoint(endIf_block);
    return nullptr;
  }
  llvm::Value* emitExpr(Expr &expr) {
    return context.emitExpr(&expr);
  }
  llvm::Value* emitStmt(Stmt *stmt) {
    if (auto expr = dynamic_cast<Expr*>(stmt))
      return emitExpr(*expr);
    auto kind = stmt->classOf();
    if (kind == NodeClass::IfStmt)
      return emitIfStmt(*static_cast<IfStmt*>(stmt));
    else if (kind == NodeClass::ForStmt)
      return emitForStmt(*static_cast<ForStmt*>(stmt));
    else if (kind == NodeClass::BreakStmt)
      return emitBreakStmt(*static_cast<BreakStmt*>(stmt));
    else if (kind == NodeClass::CompoundStmt) {
      llvm::Value *last { };
      for (auto sub_stmt : static_cast<CompoundStmt*>(stmt)->getStmts())
        last = emitStmt(sub_stmt);
      return last;
    }
    return nullptr;
  }
protected:
  CGContext &context;
  llvm::BasicBlock *end_block { };
  std::map<NodeClass, int> counters;
};
llvm::Value* CGContext::emitStmt(Stmt *stmt) {
  EmitStmt emitter(*this);
  return emitter.emitStmt(stmt);
}
}
