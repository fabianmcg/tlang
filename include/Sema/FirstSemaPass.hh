#ifndef __SEMA_NAMERESOLUTION_HH__
#define __SEMA_NAMERESOLUTION_HH__

#include "AST/Include.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Common/Utility.hh>
#include <deque>
#include <map>
#include <string>

namespace tlang::sema {
//struct FirstSemaPassAST: RecursiveASTVisitor<FirstSemaPassAST, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>, false, true> {
//  FirstSemaPassAST(ScopeContext &context, ASTContext &ctx) :
//      context(context), ctx(ctx) {
//  }
//  visit_t visitUnresolvedType(UnresolvedType *node, bool isFirst) {
//    auto parent = node->parent();
//    auto kind = parent->classOf();
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
//  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
//    add_scope(node, isFirst);
//    bool is_complete = true;
//    if (!isFirst) {
//      for (auto param : node->getParameters()) {
//        if (param->getType().isDependent() == 1)
//          is_complete = false;
//      }
//      if (node->getReturntype().isDependent() == 1)
//        is_complete = false;
//      node->getComplete() = is_complete;
//    }
//    return visit_t::visit;
//  }
//  visit_t visitModuleDecl(ModuleDecl *node, bool isFirst) {
//    return add_scope(node, isFirst);
//  }
//  visit_t visitTagDecl(TagDecl *node, bool isFirst) {
//    return add_scope(node, isFirst);
//  }
//  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
//    return add_scope(node, isFirst);
//  }
//  visit_t visitCompoundStmt(CompoundStmt *node, bool isFirst) {
//    return add_scope(node, isFirst);
//  }
//  visit_t add_scope(ASTNode *node, bool isFirst) {
//    if (isFirst)
//      context.push(node);
//    else
//      context.pop();
//    return visit_t::visit;
//  }
//  Type* make_type(Decl *decl) {
//    if (auto tag = dynamic_cast<TagDecl*>(decl)) {
//      auto kind = decl->classOf();
//      if (kind == NodeClass::StructDecl) {
//        return ctx.add_type(StructType(tag->getIdentifier(), make_ref(dynamic_cast<NamedDecl*>(decl))));
//      } else if (kind == NodeClass::EnumDecl)
//        return ctx.add_type(EnumType(tag->getIdentifier(), make_ref(dynamic_cast<NamedDecl*>(decl))));
//    }
//    throw(std::runtime_error("Invalid decl for type"));
//  }
//  ScopeContext &context;
//  ASTContext &ctx;
//};
//inline void FirstSemaPass(ASTContext &ctx, ScopeContext &sctx) {
//  FirstSemaPassAST { sctx, ctx }.traverseModuleDecl(*ctx);
//}
}
#endif
