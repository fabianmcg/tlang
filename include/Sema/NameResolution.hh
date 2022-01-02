#ifndef __SEMA_NAMERESOLUTION_HH__
#define __SEMA_NAMERESOLUTION_HH__

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
};
struct ScopeFillerAST: RecursiveASTVisitor<ScopeFillerAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
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
  visit_t visitFunctionDecl(FunctionDecl *node, bool isFirst) {
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
struct NameResolutionAST: RecursiveASTVisitor<NameResolutionAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  NameResolutionAST(ScopeContext &context) :
      context(context) {
  }
  visit_t visitDefinedType(DefinedType *node, bool isFirst) {
    auto decl = context.find(node->getIdentifier());
    auto tdecl = dynamic_cast<TagDecl*>(decl);
    if (decl && tdecl)
      node->getDecl() = tdecl;
    else
      throw(std::runtime_error("Undefined type: " + node->getIdentifier()));
    return visit_value;
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    auto decl = context.find(node->getIdentifier());
    if (decl)
      node->getDecl() = decl;
    else
      throw(std::runtime_error("Undefined name: " + node->getIdentifier()));
    return visit_value;
  }
  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
    return add_scope(node, isFirst);
  }
  visit_t visitFunctionDecl(FunctionDecl *node, bool isFirst) {
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
  ScopeContext &context;
};
struct NameResolution {
  NameResolution(ASTContext &context) :
      context(context) {
  }
  void analyze() {
    ScopeContext ctx { *context };
    ScopeFillerAST { ctx }.traverseModuleDecl(*context);
    NameResolutionAST { ctx }.traverseModuleDecl(*context);
  }
  ASTContext &context;
};
}
#endif
