#ifndef __SEMA_NAMERESOLUTION_HH__
#define __SEMA_NAMERESOLUTION_HH__

#include <AST/ASTContext.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
struct FirstSemaPassAST: ASTVisitor<FirstSemaPassAST, VisitorPattern::prePostOrder> {
  FirstSemaPassAST(ASTContext &context) :
      context(context) {
  }
//  visit_t visitUnresolvedType(UnresolvedType *node, VisitType isFirst) {
//    auto parent = node->parent();
//    auto kind = parent->classof();
//    if (isFirst) {
//      auto decl = context.find(node->getIdentifier());
//      auto tdecl = dynamic_cast<NamedDecl*>(decl);
//      if (decl && tdecl) {
//        if (kind == NodeClass::QualType) {
//          auto type = dynamic_cast<QualType*>(parent);
//          type->getType() = make_type(tdecl);
//        } else if (auto type = dynamic_cast<Type*>(parent)) {
//          if (auto underlyingPtr = type->getUnderlyingPtr()) {
//            Type *&underlying = *underlyingPtr;
//            underlying = make_type(tdecl);
//          }
//        }
//      }
////      else
////        throw(std::runtime_error("Undefined type: " + node->getIdentifier()));
//    }
//    return visit_value;
//  }
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
inline void FirstSemaPass(ASTContext &ctx) {
  FirstSemaPassAST { ctx }.traverseModuleDecl(*ctx);
}
}
#endif
