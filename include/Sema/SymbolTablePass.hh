#ifndef __SEMA_SYMBOLTABLE_HH__
#define __SEMA_SYMBOLTABLE_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
struct SymbolTableAST: RecursiveASTVisitor<SymbolTableAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, false, true> {
  SymbolTableAST(ASTContext &context) :
      context(context) {
  }
  visit_t visitNamedDecl(NamedDecl *node, bool isFirst) {
    if (isFirst) {
      if (node->isNot(NodeClass::ModuleDecl)) {
        if (auto table = table_stack.front())
          table->add(node->getIdentifier(), node);
        if (node->is(NodeClass::EnumMemberDecl)) {
          auto &table = context[top];
          table.add(node->getIdentifier(), node);
        }
      }
    }
    return visit_t::visit;
  }
  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
    top = node;
    return add_scope(node, isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitForStmt(ForStmt *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitExpr(Expr *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t add_scope(ASTNode *node, bool isFirst) {
    if (isFirst) {
      auto &table = context[node];
      if (table_stack.size() && table.parent == nullptr)
        table.parent = table_stack.front();
      table_stack.push_front(&table);
    } else
      table_stack.pop_front();
    return visit_t::visit;
  }
  ASTContext &context;
  std::deque<SymbolTable*> table_stack;
  ModuleDecl* top{};
};
inline void SymbolTablePass(ASTContext &ctx) {
  SymbolTableAST { ctx }.traverseModuleDecl(*ctx);
}
}
#endif
