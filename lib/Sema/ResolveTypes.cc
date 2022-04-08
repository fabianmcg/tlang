#include <deque>
#include <map>
#include <string>
#include <Sema/Sema.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
namespace sema {
namespace {
struct TypeVisitor: ASTVisitor<TypeVisitor, VisitorPattern::prePostOrder> {
  TypeVisitor(TypeContext &context, UniversalContext &declContext) :
      context(context), declContext(declContext) {
  }
  visit_t visitUnresolvedType(UnresolvedType *node, VisitType isFirst) {
    if (auto symbol = declContext.find(node->getIdentifier(), false)) {
      if (auto sd = dyn_cast<StructDecl>(*symbol)) {
        resolved = StructType::get(&context, sd);
        context.remove(static_cast<Type*>(node));
      }
    } else
      throw(std::runtime_error("Undefined type: " + node->getIdentifier()));
    return skip;
  }
  visit_t visitPtrType(PtrType *node, VisitType isFirst) {
    if (isFirst == postVisit)
      if (resolved) {
        resolved = PtrType::get(&context, resolved);
        context.remove(static_cast<Type*>(node));
      }
    return visit;
  }
  visit_t visitArrayType(ArrayType *node, VisitType isFirst) {
    if (isFirst == postVisit)
      if (resolved) {
        node->getUnderlying() = resolved;
        resolved = node;
      }
    return visit;
  }
  visit_t visitQualType(QualType *node, VisitType isFirst) {
    if (isFirst == postVisit) {
      if (resolved)
        node->getType() = resolved;
    }
    resolved = nullptr;
    return visit;
  }
  TypeContext &context;
  UniversalContext &declContext;
  Type *resolved { };
};
}
struct ResolveTypesVisitor: ASTVisitor<ResolveTypesVisitor, VisitorPattern::prePostOrder> {
  ResolveTypesVisitor(ASTContext &context) :
      context(context) {
  }
  visit_t visitQualType(QualType *node, VisitType isFirst) {
    if (isFirst && declContext)
      TypeVisitor { context.types(), *declContext }.traverseQualType(node);
    return skip;
  }
  visit_t visitUnitDecl(UnitDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitModuleDecl(ModuleDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitTagDecl(TagDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitFunctorDecl(FunctorDecl *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitForStmt(ForStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitLoopStmt(LoopStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t visitCompoundStmt(CompoundStmt *node, VisitType isFirst) {
    return add_scope(static_cast<UniversalContext*>(node), isFirst);
  }
  visit_t add_scope(UniversalContext *node, VisitType isFirst) {
    if (isFirst)
      declContext = node;
    else
      declContext = node->getParent();
    return visit_t::visit;
  }
  ASTContext &context;
  UniversalContext *declContext { };
};
}
void Sema::resolveTypes() {
  sema::ResolveTypesVisitor { context }.traverseProgramDecl(*context);
}
}
