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
    if (isFirst && table_stack.size()) {
      auto &ctx = *table_stack.front();
      if (auto symbol = ctx.find(node->getIdentifier(), false)) {
        node->getDecl() = *symbol;
//        if (auto dk = dynamic_cast<VariableDecl*>(decl)) {
//          node->getType() = dk->getType();
//        } else if (auto dk = dynamic_cast<FunctorDecl*>(decl)) {
//          node->getType() = dk->getReturntype();
//        } else if (auto dk = dynamic_cast<ExternFunctionDecl*>(decl)) {
//          node->getType() = dk->getReturntype();
//        }
      } else
        std::cerr << "Symbol: " << node->getIdentifier() << " doesn't exists." << std::endl;
    }
    return visit;
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
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalSymbolTable*>(node), isFirst);
  }
  visit_t add_scope(UniversalSymbolTable *node, VisitType isFirst) {
    if (isFirst)
      table_stack.push_front(node);
    else
      table_stack.pop_front();
    return visit_t::visit;
  }
  ASTContext &context;
  std::deque<UniversalSymbolTable*> table_stack;
};
}
void Sema::resolveNames() {
  sema::ResolveNamesVisitor { context }.traverseModuleDecl(*context);
}
}
