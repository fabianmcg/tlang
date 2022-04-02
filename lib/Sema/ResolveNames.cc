#include <deque>
#include <map>
#include <string>
#include <Sema/Sema.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
namespace sema {
struct ResolveNamesVisitor: ASTVisitor<ResolveNamesVisitor, VisitorPattern::prePostOrder> {
  ResolveNamesVisitor(ASTContext &context) :
      context(context) {
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, VisitType isFirst) {
    if (isFirst && declContext) {
      if (auto symbol = declContext->find(node->getIdentifier(), false)) {
        node->getDecl() = *symbol;
      } else
        std::cerr << "Symbol: " << node->getIdentifier() << " doesn't exists." << std::endl;
    }
    return visit;
  }
  visit_t visitUnitDecl(UnitDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t add_scope(UniversalSymbolTable *node, VisitType isFirst) {
    if (isFirst)
      declContext = node;
    else
      declContext = node->getParent();
    return visit_t::visit;
  }
  ASTContext &context;
  UniversalSymbolTable *declContext { };
};
}
void Sema::resolveNames() {
  sema::ResolveNamesVisitor { context }.traverseProgramDecl(*context);
}
}
