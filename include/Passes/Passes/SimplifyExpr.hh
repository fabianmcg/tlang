#ifndef PASSES_PASSES_SIMPLIFYEXPR_HH
#define PASSES_PASSES_SIMPLIFYEXPR_HH

#include <AST/Visitors/EditVisitor.hh>
#include <Passes/Impl/Pass.hh>
#include <Passes/ResultManager.hh>

namespace tlang {
struct SimplifyExpr: public PassBase<SimplifyExpr> {
  SimplifyExpr(ASTContext &context) :
      context(context) {
  }
  bool run(Expr &expr, AnyASTNodeRef ref, ResultManager &results) {
    if (auto ue = dyn_cast<UnaryOperator>(&expr)) {
      if (ue->isMinus() && ref) {
        auto me = ue->getExpr();
        if (isa<LiteralExpr>(me)) {
          ref.assign<Expr>(me);
          context.remove(&expr);
          if (auto le = dyn_cast<IntegerLiteral>(me))
            le->getValue() = -le->getValue();
          else if (auto le = dyn_cast<UIntegerLiteral>(me))
            le->getValue() = -le->getValue();
          else if (auto le = dyn_cast<FloatLiteral>(me))
            le->getValue() = -le->getValue();
        }
      }
    }
    return {};
  }
  ASTContext &context;
};
}

#endif
