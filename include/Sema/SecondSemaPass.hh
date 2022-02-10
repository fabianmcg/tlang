#ifndef __SEMA_SECOND_PASS_HH__
#define __SEMA_SECOND_PASS_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <deque>
#include <map>
#include <string>

#include "Scope.hh"

namespace tlang::sema {
struct TopDeclRef: RecursiveASTVisitor<TopDeclRef, VisitorPattern::preOrder, VisitReturn<VisitStatus>> {
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    expr = node;
    return visit_t::skip;
  }
  template <typename T>
  DeclRefExpr* find(T *node) {
    if (node)
      dynamicTraverse(node);
    return expr;
  }
  DeclRefExpr *expr { };
};
struct SecondSemaPassAST: RecursiveASTVisitor<SecondSemaPassAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, false, true> {
  SecondSemaPassAST(ScopeContext &context, ASTContext &ctx) :
      context(context), ctx(ctx) {
    memberScope.push_back(nullptr);
  }
  visit_t visitCallExpr(CallExpr *node, bool isFirst) {
    if (isFirst) {
      DeclRefExpr *refExpr = TopDeclRef { }.find(node);
      auto decl = context.find(refExpr->getIdentifier());
      if (decl)
        refExpr->getDecl() = decl;
      else
        throw(std::runtime_error("Undefined name: " + refExpr->getIdentifier()));
      for (auto arg : node->getArgs())
        dynamicTraverse(arg);
    }
    return visit_t::skip;
  }
  visit_t visitMemberExpr(MemberExpr *node, bool isFirst) {

    if (isFirst) {

    }
    return visit_t::skip;
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    if (!isFirst)
      return visit_value;
    auto decl = context.find(node->getIdentifier());
    if (decl)
      node->getDecl() = decl;
    else
      throw(std::runtime_error("Undefined name: " + node->getIdentifier()));
    if (auto var_decl = dynamic_cast<VariableDecl*>(decl)) {
      auto &type = var_decl->getType();
      node->getType() = QualType(type.getType(), (QualType::cvr_qualifiers_t) (type.getQualifiers() | QualType::Reference));
    }
    return visit_value;
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    if (!node->getComplete())
      return visit_t::skip;
    add_scope(node, isFirst);
    return visit_t::visit;
  }
  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t add_scope(ASTNode *node, bool isFirst) {
    if (isFirst)
      context.push(node);
    else
      context.pop();
    return visit_t::visit;
  }
  std::deque<Scope*> memberScope;
  ScopeContext &context;
  ASTContext &ctx;
};
inline void SecondSemaPass(ASTContext &ctx, ScopeContext &sctx) {
  SecondSemaPassAST { sctx, ctx }.traverseModuleDecl(*ctx);
}
}
#endif
