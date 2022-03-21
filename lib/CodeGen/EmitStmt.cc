#include "CodeGen/Context.hh"
#include "llvm/IR/Constants.h"
#include "Analysis/Stmt.hh"

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
    ir_params[0] = llvm::PointerType::get(emitExecFunctionType(), 0);
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
  llvm::Function* emitParallelPart() {
    if (parallelPart)
      return parallelPart;
    llvm::SmallVector<llvm::Type*, 2> ir_params(2, nullptr);
    ir_params[0] = llvm::Type::getInt32Ty(*context);
    ir_params[1] = llvm::Type::getInt32Ty(*context);
//    auto type = llvm::StructType::create(*context, "loopPartType");
//    type->setBody(ir_params);
//    parallelPartType = type;
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context), ir_params, false);
    parallelPart = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_loop_partition", context.module);
    return parallelPart;
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
    parallelNT = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "__tlang_nt", context.module);
    return parallelNT;
  }
  llvm::StructType* emitStructType(ParallelStmt &stmt) {
    llvm::StructType *type = llvm::StructType::create(*context, getStructName());
    std::vector<llvm::Type*> members { };
    auto &ac = context.astCtx.analysisContext();
    if (auto *analysis = dynamic_cast<ParallelStmtAnalysis*>(ac[&stmt])) {
      auto &sharedV = analysis->sharedVariables;
      for (auto var : sharedV) {
        auto type = var->getType().getType();
        if (type->is(NodeClass::PtrType))
          members.push_back(context.emitType(var->getType()));
        else {
          members.push_back(llvm::PointerType::get(context.emitType(var->getType()), 0));
          var->isRef = true;
        }
      }
    }
    type->setBody(members);
    return type;
  }
  llvm::Function* emitParallelSection(ParallelStmt &stmt, llvm::StructType *contextType) {
    llvm::FunctionType *function_type = emitExecFunctionType();
    auto ArgPtrType = llvm::PointerType::get(argumentsType, 0);
    auto contextPtrType = llvm::PointerType::get(contextType, 0);
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, getSectionName(), context.module);
    context[&stmt] = function;
    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(*context, "entry_block", function);
    context.builder.SetInsertPoint(entry_block);
    llvm::AllocaInst *argumentsAlloca = context->CreateAlloca(ArgPtrType, 0, "threadDataPtr");
    llvm::AllocaInst *contextAlloca = context->CreateAlloca(contextPtrType, 0, "contextPtr");
    llvm::AllocaInst *tidAlloca = context->CreateAlloca(llvm::Type::getInt32Ty(*context), 0, "tid");
    llvm::Value *bitCast = context->CreateBitCast(function->getArg(0), ArgPtrType);
    context->CreateStore(bitCast, argumentsAlloca);
    {
      auto argPtr = context->CreateLoad(ArgPtrType, argumentsAlloca);
      llvm::Value *ctxV = context->CreateStructGEP(argumentsType, argPtr, 0);
      llvm::Value *tidV = context->CreateStructGEP(argumentsType, argPtr, 1);
      ctxV = context->CreateLoad(argumentsType->getTypeAtIndex((int) 0), ctxV);
      ctxV = context->CreateBitCast(ctxV, contextPtrType);
      context->CreateStore(ctxV, contextAlloca);
      tidV = context->CreateLoad(argumentsType->getTypeAtIndex(1), tidV);
      context->CreateStore(tidV, tidAlloca);
    }
    auto &ac = context.astCtx.analysisContext();
    if (auto *analysis = dynamic_cast<ParallelStmtAnalysis*>(ac[&stmt])) {
      auto &sharedV = analysis->sharedVariables;
      auto &privateV = analysis->privateVariables;
      for (auto var : privateV)
        context.emitDecl(var);
      int idx { };
      auto ctxPtr = context->CreateLoad(contextPtrType, contextAlloca);
      for (auto var : sharedV) {
        if (var->isRef) {
          llvm::AllocaInst *alloca = context->CreateAlloca(contextType->getTypeAtIndex(idx), 0, var->getIdentifier().c_str());
          context[var] = alloca;
          llvm::Value *ctxV = context->CreateStructGEP(contextType, ctxPtr, idx);
          ctxV = context->CreateLoad(contextType->getTypeAtIndex(idx), ctxV);
          context->CreateStore(ctxV, alloca);
        } else {
          llvm::AllocaInst *alloca = static_cast<llvm::AllocaInst*>(context.emitDecl(var));
          llvm::Value *ctxV = context->CreateStructGEP(contextType, ctxPtr, idx);
          ctxV = context->CreateLoad(contextType->getTypeAtIndex(idx), ctxV);
          context->CreateStore(ctxV, alloca);
        }
        ++idx;
      }
    } else
      throw(std::runtime_error("Invalid parallel section"));
    {
      llvm::SmallVector<llvm::Value*, 1> args;
      args.push_back(context->CreateLoad(llvm::Type::getInt32Ty(*context), tidAlloca));
      context.builder.CreateCall(parallelInit, args);
    }
    emitter->emitStmt(stmt.getStmt());
    if (!context.builder.GetInsertBlock()->getTerminator()) {
      context->CreateCall(parallelExit);
      llvm::Value *value = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(*context, 0));
      context.builder.CreateRet(value);
    }
    return function;
  }
  llvm::AllocaInst* emitHeader(ParallelStmt &stmt, llvm::StructType *contextType) {
    auto FN = context.builder.GetInsertBlock()->getParent();
    auto &EB = FN->getEntryBlock();
    llvm::IRBuilder<> builder(&EB, EB.begin());
    llvm::AllocaInst *contextAlloca = builder.CreateAlloca(contextType, 0, "parallelContext" + std::to_string(counter));
    auto &ac = context.astCtx.analysisContext();
    if (auto *analysis = dynamic_cast<ParallelStmtAnalysis*>(ac[&stmt])) {
      auto &sharedV = analysis->sharedVariables;
      int idx { };
      for (auto var : sharedV) {
        llvm::Value *ctxV = context->CreateStructGEP(contextType, contextAlloca, idx);
        if (var->isRef) {
          context->CreateStore(context[var], ctxV);
        } else {
          llvm::Value *val = context->CreateLoad(context.emitType(var->getType()), context[var]);
          context->CreateStore(val, ctxV);
        }
        ++idx;
      }
    }
    return contextAlloca;
  }
  llvm::Value* emit(ParallelStmt &stmt) {
    llvm::BasicBlock *tmpBlock = context.builder.GetInsertBlock();
    emitParallelCreate();
    emitParallelInit();
    emitParallelExit();
    emitParallelSync();
    emitParallelTID();
    emitParallelNT();
    emitParallelPart();
    emitArgumentsStruct();
    llvm::StructType *args_type = emitStructType(stmt);
    llvm::AllocaInst *contextS = emitHeader(stmt, args_type);
    context.pushContext();
    llvm::Function *section = emitParallelSection(stmt, args_type);
    context.popContext();
    auto &ac = context.astCtx.analysisContext();
    if (auto *analysis = dynamic_cast<ParallelStmtAnalysis*>(ac[&stmt])) {
      auto &sharedV = analysis->sharedVariables;
      for (auto var : sharedV)
        var->isRef = false;
    }
    context.builder.SetInsertPoint(tmpBlock);
    llvm::SmallVector<llvm::Value*, 3> args;
    args.push_back(section);
    args.push_back(context->CreateBitCast(contextS, llvm::Type::getInt8PtrTy(*context)));
    args.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 1, true));
    context.builder.CreateCall(parallelCreate, args);
    ++counter;
    return nullptr;
  }
  llvm::Value* emitLoopStmt(LoopStmt &stmt) {
    struct ReduceVar {
      VariableDecl *decl { };
      llvm::Value *sharedAlloca { };
      llvm::Value *privateAlloca { };
      bool state;
    };
    llvm::Function *function = context.builder.GetInsertBlock()->getParent();
    std::vector<ReduceVar> reduceInfo;
    if (stmt.hasReducelist() && stmt.getReducelist()->getExprs().size()) {
      auto &exprs = stmt.getReducelist()->getExprs();
      for (auto expr : exprs) {
        if (auto ref = dynamic_cast<DeclRefExpr*>(expr)) {
          if (auto vd = dynamic_cast<VariableDecl*>(ref->getDecl().data())) {
            reduceInfo.push_back(ReduceVar { vd, context[vd], nullptr });
//            if (!vd->isRef)
            continue;
          }
        }
        throw(std::runtime_error("Invalid expr in reduce"));
      }
    }
    if (reduceInfo.size()) {
      auto &EB = function->getEntryBlock();
      llvm::IRBuilder<> builder(&EB, EB.begin());
      for (ReduceVar &info : reduceInfo) {
        std::string name = info.decl->getIdentifier() + ".rvar";
        info.privateAlloca = builder.CreateAlloca(context.emitType(info.decl->getType()), 0, name.c_str());
        context[info.decl] = info.privateAlloca;
        llvm::Value *load = context->CreateLoad(llvm::PointerType::get(context.emitType(info.decl->getType()), 0), info.sharedAlloca);
        load = context->CreateLoad(context.emitType(info.decl->getType()), load);
        context->CreateStore(load, info.privateAlloca);
        info.state = info.decl->isRef;
        info.decl->isRef = false;
      }
    }
    for (auto &range_stmt : stmt.getRanges()) {
      VariableDecl *for_variable =
          range_stmt->getVariable() ? range_stmt->getVariable()->getDecl().getAs<VariableDecl>() : range_stmt->getDecl();
      auto range_expr = range_stmt->getRange();
      llvm::BasicBlock *for_preamble = llvm::BasicBlock::Create(*context, emitter->makeLabel(NodeClass::LoopStmt, "LoopStmt"), function);
      llvm::BasicBlock *for_body = llvm::BasicBlock::Create(*context, emitter->makeLabel(NodeClass::LoopStmt, "LoopBody"), function);
      llvm::BasicBlock *iteration_block = llvm::BasicBlock::Create(*context, emitter->makeLabel(NodeClass::LoopStmt, "LoopIt"), function);
      llvm::BasicBlock *end_for = llvm::BasicBlock::Create(*context, emitter->makeLabel(NodeClass::LoopStmt, "EndLoopStmt"));
      emitter->incrementLabel(NodeClass::ForStmt);
      llvm::BasicBlock *tmp_block = end_for;
      std::swap(tmp_block, emitter->end_block);

      // Emit preamble
      emitter->emitBranch(for_preamble);
      llvm::AllocaInst *alloca = static_cast<llvm::AllocaInst*>(context[for_variable]);
      context.builder.SetInsertPoint(for_preamble);

      llvm::Value *start_value = emitter->emitStmt(range_expr->getStart());
      llvm::Value *end_condition = emitter->emitStmt(range_expr->getStop());
      {
        llvm::SmallVector<llvm::Value*, 2> args( { start_value, end_condition });
        auto value = context->CreateCall(parallelPart, args);
        start_value = context->CreateTrunc(value, llvm::Type::getInt32Ty(*context));
        auto *tmp = context->CreateLShr(value, (uint64_t) 32);
        end_condition = context->CreateTrunc(tmp, llvm::Type::getInt32Ty(*context));
      }
      context.builder.CreateStore(start_value, alloca);
      llvm::Value *tmp = context.builder.CreateICmpSLT(start_value, end_condition);
      context.builder.CreateCondBr(tmp, for_body, end_for);

      // Emit For body
      //      context.builder.CreateBr(for_body);
      context.builder.SetInsertPoint(for_body);
      emitter->emitStmt(stmt.getBody());
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
      std::swap(tmp_block, emitter->end_block);
    }
    if (reduceInfo.size()) {
      for (ReduceVar &info : reduceInfo) {
        info.decl->isRef = info.state;
        context[info.decl] = info.sharedAlloca;
        llvm::Value *pValue = context->CreateLoad(context.emitType(info.decl->getType()), info.privateAlloca);
        llvm::Value *sValue = context->CreateLoad(llvm::PointerType::get(context.emitType(info.decl->getType()), 0), info.sharedAlloca);
//        context->CreateStore(sValue, info.privateAlloca);
        auto value = context->CreateAtomicRMW(llvm::AtomicRMWInst::FAdd, sValue, pValue, llvm::MaybeAlign(),
            llvm::AtomicOrdering::SequentiallyConsistent);
      }
    }
    if (!stmt.getNowait())
      context->CreateCall(parallelSync);
    return nullptr;
  }
  llvm::Value* emitSyncStmt(SyncStmt &stmt) {
    if (parallelSync)
      return context->CreateCall(parallelSync);
    return nullptr;
  }
  llvm::Value* emitAtomicStmt(AtomicStmt &stmt) {
    auto RHS = emitter->emitStmt(stmt.getRhs());
    auto LHS = context.emitExpr(stmt.getLhs(), false);
    auto value = context->CreateAtomicRMW(llvm::AtomicRMWInst::FAdd, LHS, RHS, llvm::MaybeAlign(),
        llvm::AtomicOrdering::SequentiallyConsistent);
    return value;
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
  llvm::Function *parallelPart { };
  llvm::StructType *parallelPartType { };
  llvm::StructType *argumentsType { };
};
struct EmitStmt {
  template <typename >
  friend struct EmitParallel;
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
//      emitStmt(range_expr->getStart());
      auto start_value = emitStmt(range_expr->getStart());
      context.builder.CreateStore(start_value, alloca);
      llvm::Value *end_condition = emitStmt(range_expr->getStop());
      llvm::Value *tmp = context.builder.CreateICmpSLT(start_value, end_condition);
      context.builder.CreateCondBr(tmp, for_body, end_for);

      // Emit For body
//      context.builder.CreateBr(for_body);
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
  llvm::Value* emitLoopStmt(LoopStmt &stmt) {
    return parallelEmitter.emitLoopStmt(stmt);
  }
  llvm::Value* emitSyncStmt(SyncStmt &stmt) {
    return parallelEmitter.emitSyncStmt(stmt);
  }
  llvm::Value* emitAtomicStmt(AtomicStmt &stmt) {
    return parallelEmitter.emitAtomicStmt(stmt);
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
    else if (kind == NodeClass::LoopStmt)
      return emitLoopStmt(*static_cast<LoopStmt*>(stmt));
    else if (kind == NodeClass::SyncStmt)
      return emitSyncStmt(*static_cast<SyncStmt*>(stmt));
    else if (kind == NodeClass::AtomicStmt)
      return emitAtomicStmt(*static_cast<AtomicStmt*>(stmt));
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
