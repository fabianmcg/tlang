#include "CodeGen/CodeGen.hh"
#include "llvm/ADT/APFloat.h"
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
          TmpB.CreateStore(alloca, context[node]);
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
  visit_t visitIfStmt(IfStmt *node, bool isFirst) {
    using namespace llvm;

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
