#ifndef __SEMA_SCOPE_HH__
#define __SEMA_SCOPE_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
struct Scope {
  using scope_map_t = std::map<std::string, NamedDecl*>;
  scope_map_t local_scope { };
  void add(NamedDecl *decl) {
    if (decl) {
      auto usr = decl->usr();
      auto &nd = local_scope[usr];
      if (!nd)
        nd = decl;
      else
        throw(std::runtime_error("Redefinition of " + to_string(decl->kind) + " " + usr));
    }
  }
  NamedDecl* find(const std::string &identifier) {
    auto it = local_scope.find(identifier);
    if (it == local_scope.end())
      return nullptr;
    return it->second;
  }
};
struct ScopeContext {
  std::map<ASTNode*, Scope> scopes;
  std::deque<Scope*> scope_stack;
  ScopeContext(ModuleDecl *decl) {
    push(decl);
  }
  void push(ASTNode *node) {
    auto &scope = scopes[node];
    scope_stack.push_front(&scope);
  }
  void pop() {
    if (scope_stack.size() > 1)
      scope_stack.pop_front();
  }
  Scope* operator*() {
    return scope_stack.front();
  }
  const Scope* operator*() const {
    return scope_stack.front();
  }
  void add(NamedDecl *decl) {
    (**this)->add(decl);
  }
  NamedDecl* find(const std::string &identifier) {
    for (auto &scope : scope_stack) {
      if (auto decl = scope->find(identifier))
        return decl;
    }
    return nullptr;
  }
  Scope* find(ASTNode *node) {
    auto it = scopes.find(node);
    if (it == scopes.end())
      return nullptr;
    return &(it->second);
  }
};
struct ScopeFillerAST: RecursiveASTVisitor<ScopeFillerAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, false, true> {
  ScopeFillerAST(ScopeContext &context) :
      context(context) {
  }
  visit_t visitNamedDecl(NamedDecl *node, bool isFirst) {
    if (isFirst) {
      if (node->isNot(NodeClass::ModuleDecl))
        context.add(node);
    }
    return visit_t::visit;
  }
  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitExpr(Expr *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    return visit_t::skip;
  }
  visit_t add_scope(ASTNode *node, bool isFirst) {
    if (isFirst)
      context.push(node);
    else
      context.pop();
    return visit_t::visit;
  }
  ScopeContext &context;
};
}
#endif
