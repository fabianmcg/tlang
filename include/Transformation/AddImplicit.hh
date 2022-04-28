#ifndef TRANSFORMATION_ADDIMPLICIT_HH
#define TRANSFORMATION_ADDIMPLICIT_HH

#include <AST/Visitors/ASTVisitor.hh>
#include <AST/Api.hh>
#include <Passes/Impl/Pass.hh>
#include <Passes/ResultManager.hh>
#include <Support/Enumerate.hh>

namespace tlang {
struct AddImplicitExpr: public PassBase<AddImplicitExpr>, public ASTContextReference {
  using ASTContextReference::ASTContextReference;
  struct Visitor: ASTVisitor<Visitor, VisitorPattern::postOrder>, public ASTContextReference {
    using ASTContextReference::ASTContextReference;
    visit_t visitUnaryOperator(UnaryOperator *expr) {
      auto op = expr->getOp();
      auto &lhs = expr->getExpr();
      if (lhs->getType().isReference() && op == UnaryOperator::Dereference)
        lhs = builder.CreateImplicitCast(lhs, lhs->getType().modQuals(QualType::Reference));
      return visit;
    }
    visit_t visitBinaryOperator(BinaryOperator *expr) {
      auto op = expr->getOp();
      auto &lhs = expr->getLhs();
      auto &rhs = expr->getRhs();
      if (rhs->getType().isReference())
        rhs = builder.CreateImplicitCast(rhs, rhs->getType().modQuals());
      if (lhs->getType().isReference() && op != BinaryOperator::Assign)
        lhs = builder.CreateImplicitCast(lhs, lhs->getType().modQuals());
      return visit;
    }
    visit_t visitCallExpr(CallExpr *cexpr) {
      auto re = dyn_cast<DeclRefExpr>(cexpr->getCallee());
      assert(re);
      auto fd = dyn_cast<FunctorDecl>(re->getDecl().data());
      assert(fd);
      auto &parameters = fd->getParameters();
      for (auto [i, expr] : tlang::enumerate(cexpr->getArgs())) {
        if (i < parameters.size())
          if (expr->getType().isReference() && !parameters[i]->getType().isReference())
            expr = builder.CreateImplicitCast(expr, expr->getType().modQuals());
      }
      return visit;
    }
    visit_t visitArrayExpr(ArrayExpr *expr) {
      if (expr->getArray()->getType().isReference()) {
        if (auto re = dyn_cast<DeclRefExpr>(expr->getArray())) {
          if (auto vd = dyn_cast<VariableDecl>(re->getDecl().data())) {
            if (isa<ArrayType>(vd->getType().getType())) {
              expr->getArray()->getType() = expr->getArray()->getType().modQuals();
            } else
              expr->getArray() = builder.CreateImplicitCast(expr->getArray(), expr->getArray()->getType().modQuals());
          } else
            expr->getArray() = builder.CreateImplicitCast(expr->getArray(), expr->getArray()->getType().modQuals());
        } else
          expr->getArray() = builder.CreateImplicitCast(expr->getArray(), expr->getArray()->getType().modQuals());
      }
      for (auto &index : expr->getIndex())
        if (index->getType().isReference())
          index = builder.CreateImplicitCast(index, index->getType().modQuals());
      return visit;
    }
    visit_t visitVariableDecl(VariableDecl *decl) {
      if (decl->getInit()) {
        if (!decl->getType().isReference() && decl->getInit()->getType().isReference())
          decl->getInit() = builder.CreateImplicitCast(decl->getInit(), decl->getInit()->getType().modQuals());
      }
      return visit;
    }
    visit_t visitImplicitCastExpr(ImplicitCastExpr *expr) {
      if (expr->getType().getType() != expr->getExpr()->getType().getType() && expr->getExpr()->getType().isReference()) {
        expr->getExpr() = builder.CreateImplicitCast(expr->getExpr(), expr->getExpr()->getType().modQuals());
      }
      return visit;
    }
    visit_t visitReduceExpr(ReduceExpr *expr) {
      if (expr->getExpr()->getType().isReference()) {
        expr->getExpr() = builder.CreateImplicitCast(expr->getExpr(), expr->getExpr()->getType().modQuals());
      }
      return visit;
    }
    visit_t visitReturnStmt(ReturnStmt *stmt) {
      auto &expr = stmt->getReturn();
      if (expr->getType().isReference()) {
        expr = builder.CreateImplicitCast(expr, expr->getType().modQuals());
      }
      return visit;
    }
    visit_t visitAtomicStmt(AtomicStmt *stmt) {
      auto &lhs = stmt->getLhs();
      if (isa<PtrType>(lhs->getType().getType()) && lhs->getType().isReference()) {
        lhs = builder.CreateImplicitCast(lhs, lhs->getType().modQuals());
      }
      auto &rhs = stmt->getRhs();
      if (rhs->getType().isReference()) {
        rhs = builder.CreateImplicitCast(rhs, rhs->getType().modQuals());
      }
      return visit;
    }
    visit_t visitRangeExpr(RangeExpr *stmt) {
      auto toRvalue = [&](Expr *&expr) {
        if (expr && expr->getType().isReference())
          expr = builder.CreateImplicitCast(expr, expr->getType().modQuals());
      };
      toRvalue(stmt->getStart());
      toRvalue(stmt->getStep());
      toRvalue(stmt->getStop());
      return visit;
    }
    ASTApi builder { context };
  };
  bool run(ModuleDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
    Visitor { context }.traverseModuleDecl(&decl);
    return true;
  }
};
}
#endif
