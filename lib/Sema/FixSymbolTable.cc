#include <deque>
#include <map>
#include <string>
#include <Sema/Sema.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
namespace sema {
struct SymbolTableVisitor: ASTVisitor<SymbolTableVisitor, VisitorPattern::prePostOrder> {
  SymbolTableVisitor(ASTContext &context) :
      context(context) {
  }
  visit_t visitDecl(Decl *node, VisitType isFirst) {
    if (isFirst == preVisit) {
      node->getDeclContext() = getParentContext();
    }
    return visit;
  }
  visit_t visitUnitDecl(UnitDecl *node, VisitType isFirst) {
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    topContext = isFirst == preVisit ? node : nullptr;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    if (isFirst == preVisit)
      node->getTopContext() = topContext;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    if (isFirst == preVisit)
      node->getTopContext() = topContext;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    if (isFirst == preVisit)
      node->getTopContext() = topContext;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    if (isFirst == preVisit)
      node->getTopContext() = topContext;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    if (isFirst == preVisit)
      node->getTopContext() = topContext;
    return addScope(static_cast<UniversalContext*>(node), isFirst);
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
  visit_t addScope(UniversalContext *table, VisitType isFirst) {
    if (isFirst) {
      table->getParent() = getParentContext();
      table_stack.push_front(table);
    } else
      table_stack.pop_front();
    return visit_t::visit;
  }
  inline UniversalContext* getParentContext() {
    return table_stack.size() ? table_stack.front() : nullptr;
  }
  ASTContext &context;
  ModuleDecl *topContext = nullptr;
  std::deque<UniversalContext*> table_stack;
};
}
void Sema::resolveSymbolTables(UnitDecl *node) {
  sema::SymbolTableVisitor { *context }.traverseUnitDecl(node);
}
void Sema::completeTable() {
  sema::SymbolTableVisitor { *context }.traverseUniverseDecl(**context);
}
}
