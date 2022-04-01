#ifndef SEMA_SYMBOLTABLE_HH
#define SEMA_SYMBOLTABLE_HH

#include <AST/Visitors/ASTVisitor.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
struct SymbolTableAST: ASTVisitor<SymbolTableAST, VisitorPattern::prePostOrder> {
  SymbolTableAST(ASTContext &context) :
      context(context) {
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    return addScope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitVariableDecl(VariableDecl *node, VisitType isFirst) {
    return visit_t::skip;
  }
  visit_t visitExpr(Expr *node, VisitType isFirst) {
    return visit_t::skip;
  }
  visit_t visitQualType(QualType *node, VisitType isFirst) {
    return visit_t::skip;
  }
  visit_t addScope(UniversalSymbolTable *table, VisitType isFirst) {
    if (isFirst) {
      if (table_stack.size() && table->getParent() == nullptr)
        table->getParent() = table_stack.front();
      table_stack.push_front(table);
    } else
      table_stack.pop_front();
    return visit_t::visit;
  }
  ASTContext &context;
  std::deque<UniversalSymbolTable*> table_stack;
};
inline void SymbolTablePass(ASTContext &ctx) {
  SymbolTableAST { ctx }.traverseModuleDecl(*ctx);
}
}

#endif
