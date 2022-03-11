#include "CodeGen/CodeGen.hh"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace tlang::codegen {
using namespace _astnp_;
struct CodeGenVisitor: RecursiveASTVisitor<CodeGenVisitor, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  CodeGenVisitor(CGContext &context) :
      context(context) {
  }
  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitExternFunctionDecl(ExternFunctionDecl *node, bool isFirst) {
    using namespace llvm;
    add_scope(node, isFirst);
    if (isFirst) {
      auto &fp = node->getParameters();
      std::vector<llvm::Type*> parameters(fp.size(), nullptr);
      for (size_t i = 0; i < fp.size(); ++i) {
        parameters[i] = context.emitType(fp[i]);
      }
      auto rt = context.emitType(node->getReturntype());
      llvm::FunctionType *FT = llvm::FunctionType::get(rt, parameters, false);
      Function *F = Function::Create(FT, Function::ExternalLinkage, node->getIdentifier(), context.module);
      context[node] = F;
    }
    return visit_value;
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    using namespace llvm;
    add_scope(node, isFirst);
    if (isFirst) {
      auto &fp = node->getParameters();
      std::vector<llvm::Type*> parameters(fp.size(), nullptr);
      for (size_t i = 0; i < fp.size(); ++i) {
        parameters[i] = context.emitType(fp[i]->getType());
      }
      auto rt = context.emitType(node->getReturntype());
      llvm::FunctionType *FT = llvm::FunctionType::get(rt, parameters, false);
      Function *F = Function::Create(FT, Function::ExternalLinkage, node->getIdentifier(), context.module);
      context[node] = F;
      unsigned Idx = 0;
      for (auto &Arg : F->args()) {
        Arg.setName(fp[Idx]->getIdentifier());
        context[fp[Idx++]] = &Arg;
      }
      BasicBlock *BB = BasicBlock::Create(*context, "entry", F);
      context.builder.SetInsertPoint(BB);
    } else {
      auto FT = static_cast<Function*>(context[node]);
      if (FT->getReturnType()->isVoidTy())
        context.builder.CreateRetVoid();
      verifyFunction(*FT);
    }
    return visit_value;
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    using namespace llvm;
    if (scope_stack.size() && isFirst) {
      if (auto fd = dynamic_cast<FunctionDecl*>(scope_stack.front())) {
        auto function = static_cast<Function*>(context[fd]);
        IRBuilder<> TmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
        AllocaInst *alloca = TmpB.CreateAlloca(context.emitType(node->getType()), 0, node->getIdentifier().c_str());
        if (node->is(NodeClass::ParameterDecl))
          TmpB.CreateStore(context[node], alloca);
        context[node] = alloca;
      }
    }
    return visit_value;
  }
  visit_t visitExpr(Expr *node, bool isFirst) {
    using namespace llvm;
    if (scope_stack.size() && isFirst)
      if (auto fd = dynamic_cast<FunctionDecl*>(scope_stack.front()))
        context.emitExpr(node);
    return visit_t::skip;
  }
  visit_t visitBreakStmt(BreakStmt *node, bool isFirst) {
    using namespace llvm;
    if (isFirst && insideLoop) {
      if (auto bb = static_cast<BasicBlock*>(context[insideLoop]))
        context[node] = context.builder.CreateBr(bb);
    }
    return visit_t::skip;
  }
  visit_t visitIfStmt(IfStmt *node, bool isFirst) {
    using namespace llvm;
    if (isFirst) {
      auto cond = context.emitExpr(node->getCondition());
      auto CondV = context.builder.CreateICmpNE(cond, ConstantInt::get(cond->getType(), 0), "ifcond");

      Function *TheFunction = context.builder.GetInsertBlock()->getParent();

      // Create blocks for the then and else cases.  Insert the 'then' block at the
      // end of the function.
      BasicBlock *ThenBB = BasicBlock::Create(*context, "then", TheFunction);
      BasicBlock *ElseBB = nullptr;
      if (node->getElse()) {
        ElseBB = BasicBlock::Create(*context, "else");
        context[node->getElse()] = ElseBB;
      }
      BasicBlock *MergeBB = BasicBlock::Create(*context, "ifcont");
      if (ElseBB)
        context.builder.CreateCondBr(CondV, ThenBB, ElseBB);
      else
        context.builder.CreateCondBr(CondV, ThenBB, MergeBB);

      // Emit then value.
      context.builder.SetInsertPoint(ThenBB);

      dynamicTraverse(node->getThen());
//      if (ThenBB->getTerminator())
        context.builder.CreateBr(MergeBB);
      // Codegen of 'Then' can change the current block, update ThenBB for the PHI.
      ThenBB = context.builder.GetInsertBlock();

      // Emit else block.
      if (ElseBB) {
        TheFunction->getBasicBlockList().push_back(ElseBB);
        context.builder.SetInsertPoint(ElseBB);
        dynamicTraverse(node->getElse());
        context.builder.CreateBr(MergeBB);
        context[node->getElse()] = ElseBB = context.builder.GetInsertBlock();
      }
      // Emit merge block.
      TheFunction->getBasicBlockList().push_back(MergeBB);
      context.builder.SetInsertPoint(MergeBB);
      context[node] = MergeBB;
    }
    return visit_t::skip;
  }
  visit_t visitForStmt(ForStmt *node, bool isFirst) {
    using namespace llvm;
    if (isFirst) {
      insideLoop = node;
      Function *TheFunction = context.builder.GetInsertBlock()->getParent();
      auto range = node->getRange(0);
      // Create an alloca for the variable in the entry block.
      AllocaInst *Alloca;
      if (range->getVariable()) {
        auto d = context[range->getVariable()->getDecl().data()];
        Alloca = static_cast<AllocaInst*>(d);
      } else {
        visitVariableDecl(range->getDecl(), true);
        auto d = context[range->getDecl()];
        Alloca = static_cast<AllocaInst*>(d);
      }

      // Emit the start code first, without 'variable' in scope.
      dynamicTraverse(range->getRange()->getStart());
      auto StartVal = context[range->getRange()->getStart()];

      // Store the value into the alloca.
      context.builder.CreateStore(StartVal, Alloca);

      // Make the new basic block for the loop header, inserting after current
      // block.
      BasicBlock *LoopBB = BasicBlock::Create(*context, "loop", TheFunction);
      BasicBlock *AfterBB = BasicBlock::Create(*context, "afterloop");
      context[node] = AfterBB;

      // Insert an explicit fall through from the current block to the LoopBB.
      context.builder.CreateBr(LoopBB);

      // Start insertion in LoopBB.
      context.builder.SetInsertPoint(LoopBB);
      context[node->getBody()] = LoopBB;

      // Within the loop, the variable is defined equal to the PHI node.  If it
      // shadows an existing variable, we have to restore it, so save it now.
//      AllocaInst *OldVal = NamedValues[VarName];
//      NamedValues[VarName] = Alloca;

      // Emit the body of the loop.  This, like any other expr, can change the
      // current BB.  Note that we ignore the value computed by the body, but don't
      // allow an error.
      dynamicTraverse(node->getBody());

      // Emit the step value.
      Value *StepVal = StepVal = ConstantInt::get(context.emitType(range->getRange()->getStart()->getType()), 1);

      // Compute the end condition.
      dynamicTraverse(range->getRange()->getStop());
      Value *EndCond = context[range->getRange()->getStop()];

      // Reload, increment, and restore the alloca.  This handles the case where
      // the body of the loop mutates the variable.
      Value *CurVar = context.builder.CreateLoad(Alloca->getAllocatedType(), Alloca);
      Value *NextVar = context.builder.CreateAdd(CurVar, StepVal, "nextvar");
      context.builder.CreateStore(NextVar, Alloca);

      // Convert condition to a bool by comparing non-equal to 0.0.
      EndCond = context.builder.CreateICmpSLT(NextVar, EndCond, "loopcond");
      // Create the "after loop" block and insert it.
      // Insert the conditional branch into the end of LoopEndBB.
      context.builder.CreateCondBr(EndCond, LoopBB, AfterBB);
      // Any new code will be inserted in AfterBB.
      TheFunction->getBasicBlockList().push_back(AfterBB);
      context.builder.SetInsertPoint(AfterBB);
      context[node] = AfterBB;
      // Restore the unshadowed variable.
//      if (OldVal)
//        NamedValues[VarName] = OldVal;
//      else
//        NamedValues.erase(VarName);

    } else
      insideLoop = nullptr;
    return visit_t::skip;
  }
  visit_t add_scope(ASTNode *node, bool isFirst) {
    if (isFirst)
      scope_stack.push_front(node);
    else
      scope_stack.pop_front();
    return visit_t::visit;
  }
  CGContext &context;
  std::deque<ASTNode*> scope_stack;
  Stmt *insideLoop = nullptr;
};
void CodeGen::generate(ModuleDecl *module) {
  CGContext ctx = makeContext();
  CodeGenVisitor { ctx }.traverseModuleDecl(module);
}
}

//  visit_t visitStructDecl(StructDecl *node, bool isFirst) {
//    if (isFirst) {
//      std::vector<llvm::Type*> parameters;
//      for (auto child : **static_cast<DeclContext*>(node))
//        if (child->is(NodeClass::MemberDecl)) {
//          MemberDecl *decl = dynamic_cast<MemberDecl*>(child);
//          parameters.push_back(make(decl->getType()));
//        }
//      llvm::StructType *Struct;
//      auto &type = type_translation_table[node];
//      if (!type) {
//        Struct = llvm::StructType::create(context, node->getIdentifier());
//        type = Struct;
//      } else
//        Struct = llvm::cast<llvm::StructType>(type);
//      Struct->setBody(parameters);
//    }
//    return visit_value;
//  }
