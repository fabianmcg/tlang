#ifndef TRANSFORMATION_FIXAS_HH
#define TRANSFORMATION_FIXAS_HH

#include <AST/Visitors/ASTVisitor.hh>
#include <AST/Api.hh>
#include <Passes/Impl/Pass.hh>
#include <Passes/ResultManager.hh>
#include <Support/Enumerate.hh>

namespace tlang {
struct FixAddressSpace: public PassBase<FixAddressSpace>, public CompilerInvocationRef {
  using CompilerInvocationRef::CompilerInvocationRef;
  struct Visitor: ASTVisitor<Visitor, VisitorPattern::prePostOrder>, public CompilerInvocationRef {
    using CompilerInvocationRef::CompilerInvocationRef;
    visit_t visitVariableDecl(VariableDecl *decl, VisitType kind) {
      if (kind == postVisit)
        return visit;
      if (decl->getStorage() == VariableDecl::Shared)
        decl->getType().getAddressSpace() = 3;
      return visit;
    }
    visit_t visitDeclRefExpr(DeclRefExpr *expr, VisitType kind) {
      if (kind == postVisit)
        return visit;
      if (auto vd = dyn_cast<VariableDecl>(expr->getDecl().data())) {
        if (vd->getType().getAddressSpace())
          expr->getType().getAddressSpace() = vd->getType().getAddressSpace();
      }
      return visit;
    }
    visit_t visitImplicitCastExpr(ImplicitCastExpr *expr, VisitType kind) {
      if (kind == postVisit)
        if (int as = expr->getExpr()->getType().getAddressSpace())
          expr->getType().getAddressSpace() = as;
      return visit;
    }
    visit_t visitArrayExpr(ArrayExpr *expr, VisitType kind) {
      if (kind == postVisit)
        if (int as = expr->getArray()->getType().getAddressSpace())
          expr->getType().getAddressSpace() = as;
      return visit;
    }
    visit_t visitBinaryOperator(BinaryOperator *expr, VisitType kind) {
      if (kind == postVisit)
        if (expr->getOp() == BinaryOperator::Assign)
          if (int as = expr->getLhs()->getType().getAddressSpace())
            expr->getType().getAddressSpace() = as;
      return visit;
    }
    visit_t visitUnaryOperator(UnaryOperator *expr, VisitType kind) {
      if (kind == postVisit)
        if (int as = expr->getExpr()->getType().getAddressSpace())
          expr->getType().getAddressSpace() = as;
      return visit;
    }
  };
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
    if (decl.getBackend() != Backend::Generic)
      Visitor { CI }.traverseUnitDecl(&decl);
    return true;
  }
};
}

#endif
