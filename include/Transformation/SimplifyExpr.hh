#ifndef PASSES_PASSES_SIMPLIFYEXPR_HH
#define PASSES_PASSES_SIMPLIFYEXPR_HH

#include <AST/Visitors/EditVisitor.hh>
#include <AST/Api.hh>
#include <Passes/Impl/Pass.hh>
#include <Passes/ResultManager.hh>
#include <Support/Enumerate.hh>

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
    } else if (auto ae = dyn_cast<ArrayExpr>(&expr)) {
      auto array = ae->getArray();
      if (ae->getIndex().size() <= 1)
        return true;
      if (auto ref = dyn_cast<DeclRefExpr>(array))
        if (auto vd = dyn_cast<VariableDecl>(ref->getDecl().data())) {
          if (auto at = dyn_cast<ArrayType>(vd->getType().getType())) {
            auto indexes = std::move(ae->getIndex());
            auto &dims = at->getLayout();
            Expr *index { };
            for (auto [i, indx] : tlang::enumerate(indexes)) {
              Expr *mult;
              if (i < dims.size() - 1) {
                index = !index ? indx : builder.CreateBinOp(BinaryOperator::Plus, index, indx);
                index = builder.CreateBinOp(BinaryOperator::Multiply, index, dims[i + 1]);
              } else
                index = builder.CreateBinOp(BinaryOperator::Plus, index, indx);
            }
            ae->getIndex() = List<Expr*>( { index });
          }
        }
    }
    return {};
  }
  ASTContext &context;
  ASTApi builder { context };
};
}

#endif
