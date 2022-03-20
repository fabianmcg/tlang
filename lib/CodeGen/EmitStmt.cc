#include "CodeGen/Context.hh"
#include "llvm/IR/Constants.h"

namespace tlang::codegen {
template <typename StmtEmitter>
struct EmitParallel {
  friend struct EmitStmt;
  EmitParallel(CGContext &context, StmtEmitter *emitter) :
      context(context), emitter(emitter) {
  }
  inline std::string name(const std::string &preffix) const {
    return preffix + std::to_string(counter);
  }
  inline std::string getSectionName() const {
    return name("__parallel_section");
  }
  inline std::string getStructName() const {
    return name("threadCtxType.");
  }
  llvm::StructType* emitArgumentsStruct() {
    if (argumentsType)
      return argumentsType;
    llvm::StructType *type = llvm::StructType::create(*context, "threadArgType");
    std::vector<llvm::Type*> members { };
    members.push_back(llvm::Type::getInt8PtrTy(*context));
    members.push_back(llvm::Type::getInt32Ty(*context));
    type->setBody(members);
    argumentsType = type;
    return type;
  }
  llvm::FunctionType* emitExecFunctionType() {
    llvm::SmallVector<llvm::Type*, 1> ir_params(1, nullptr);
    ir_params[0] = llvm::Type::getInt8PtrTy(*context, 0);
    return llvm::FunctionType::get(llvm::Type::getInt8PtrTy(*context, 0), ir_params, false);
  }
  llvm::Function* emitParallelCreate() {
    if (parallelCreate)
      return parallelCreate;
    std::vector<llvm::Type*> ir_params(3, nullptr);
    ir_params[0] = emitExecFunctionType();
    ir_params[1] = llvm::Type::getInt8PtrTy(*context, 0);
    ir_params[2] = llvm::Type::getInt32Ty(*context);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context.context), ir_params, false);
    parallelCreate = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_create_parallel", context.module);
    return parallelCreate;
  }
  llvm::Function* emitParallelInit() {
    if (parallelInit)
      return parallelInit;
    llvm::SmallVector<llvm::Type*, 1> ir_params(1, nullptr);
    ir_params[0] = llvm::Type::getInt32Ty(*context);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context.context), ir_params, false);
    parallelInit = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_init_parallel", context.module);
    return parallelInit;
  }
  llvm::Function* emitParallelExit() {
    if (parallelExit)
      return parallelExit;
    llvm::SmallVector<llvm::Type*, 1> ir_params(0, nullptr);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context.context), ir_params, false);
    parallelExit = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_exit_parallel", context.module);
    return parallelExit;
  }
  llvm::Function* emitParallelSync() {
    if (parallelSync)
      return parallelSync;
    llvm::SmallVector<llvm::Type*, 1> ir_params(0, nullptr);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getVoidTy(context.context), ir_params, false);
    parallelSync = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_sync", context.module);
    return parallelSync;
  }
  llvm::Function* emitParallelTID() {
    if (parallelTid)
      return parallelTid;
    llvm::SmallVector<llvm::Type*, 1> ir_params(0, nullptr);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), ir_params, false);
    parallelTid = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_tid", context.module);
    return parallelTid;
  }
  llvm::Function* emitParallelNT() {
    if (parallelNT)
      return parallelNT;
    llvm::SmallVector<llvm::Type*, 1> ir_params(0, nullptr);
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), ir_params, false);
    parallelNT = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_nt__tlang_nt", context.module);
    return parallelNT;
  }
  llvm::StructType* emitStructType(ParallelStmt &stmt) {
    llvm::StructType *type = llvm::StructType::create(*context, getStructName());
    std::vector<llvm::Type*> members { };
    members.push_back(llvm::Type::getInt32Ty(*context));
    type->setBody(members);
    return type;
  }
  llvm::Function* emitParallelSection(ParallelStmt &stmt, llvm::StructType *contextType) {
    llvm::FunctionType *function_type = emitExecFunctionType();
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, getSectionName(), context.module);
    context[&stmt] = function;
    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(*context, "entry_block", function);
    context.builder.SetInsertPoint(entry_block);
    llvm::AllocaInst *argumentsAlloca = context->CreateAlloca(argumentsType, 0, "threadData");
    llvm::AllocaInst *contextAlloca = context->CreateAlloca(contextType, 0, "context");
    llvm::AllocaInst *tidAlloca = context->CreateAlloca(llvm::Type::getInt32Ty(*context), 0, "tid");
    llvm::Value *bitCast = context->CreateBitCast(function->getArg(0), argumentsType);
    context->CreateStore(bitCast, argumentsAlloca);
    {
      llvm::Value *ctxV = context->CreateStructGEP(argumentsType, argumentsAlloca, 0);
      llvm::Value *tidV = context->CreateStructGEP(argumentsType, argumentsAlloca, 1);
      ctxV = context->CreateBitCast(ctxV, contextType);
      context->CreateStore(ctxV, contextAlloca);
      context->CreateStore(tidV, tidAlloca);
    }
    if (stmt.getSharedvariables().size() || stmt.getReferencedshared().size()) {

    }
    {
      llvm::SmallVector<llvm::Value*, 1> args;
      args.push_back(tidAlloca);
      context.builder.CreateCall(parallelInit, args);
    }
    if (!context.builder.GetInsertBlock()->getTerminator()) {
      context->CreateCall(parallelExit);
      llvm::Value *value = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*context, 0));
      context.builder.CreateRet(value);
    }
    return function;
  }
  llvm::Value* emit(ParallelStmt &stmt) {
    llvm::BasicBlock *tmpBlock = context.builder.GetInsertBlock();
    emitParallelCreate();
    emitParallelInit();
    emitParallelExit();
    emitParallelSync();
    emitParallelTID();
    emitParallelNT();
    emitArgumentsStruct();
    llvm::StructType *args_type = emitStructType(stmt);
    emitParallelSection(stmt, args_type);
    context.builder.SetInsertPoint(tmpBlock);
    ++counter;
    return nullptr;
  }
private:
  CGContext &context;
  StmtEmitter *emitter;
  size_t counter { };
  llvm::Function *parallelCreate { };
  llvm::Function *parallelInit { };
  llvm::Function *parallelExit { };
  llvm::Function *parallelSync { };
  llvm::Function *parallelTid { };
  llvm::Function *parallelNT { };
  llvm::StructType *argumentsType { };
};
struct EmitStmt {
  EmitStmt(CGContext &context) :
      context(context), parallelEmitter(context, nullptr) {
    parallelEmitter.emitter = this;
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
  llvm::Value* emitParallelStmt(ParallelStmt &stmt) {
    return parallelEmitter.emit(stmt);
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
    else if (kind == NodeClass::ParallelStmt)
      return emitParallelStmt(*static_cast<ParallelStmt*>(stmt));
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
  EmitParallel<EmitStmt> parallelEmitter;
};
llvm::Value* CGContext::emitStmt(Stmt *stmt) {
  EmitStmt emitter(*this);
  return emitter.emitStmt(stmt);
}
}
