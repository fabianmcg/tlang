#include <AST/Api.hh>

namespace tlang {

ExternFunctionDecl* ASTApi::CreateExternFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters) {
  ExternFunctionDecl function = ExternFunctionDecl(FunctorDecl(identifier, std::move(returnType), std::move(parameters), nullptr));
  function.setType(context.types());
  return context.create<ExternFunctionDecl>(std::move(function));
}
FunctionDecl* ASTApi::CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters) {
  FunctionDecl function = FunctionDecl(FunctorDecl(identifier, std::move(returnType), std::move(parameters), context.make<CompoundStmt>()));
  function.setType(context.types());
  auto fn = context.create<FunctionDecl>(std::move(function));
  static_cast<UniversalContext*>(fn->getBody())->getParent() = static_cast<UniversalContext*>(fn);
  return fn;
}
FunctionDecl* ASTApi::CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters,
    CompoundStmt *stmt) {
  FunctionDecl function = FunctionDecl(FunctorDecl(identifier, std::move(returnType), std::move(parameters), stmt));
  function.setType(context.types());
  auto fn = context.create<FunctionDecl>(std::move(function));
  if (stmt)
    static_cast<UniversalContext*>(fn->getBody())->getParent() = static_cast<UniversalContext*>(fn);
  return fn;
}
ParameterDecl* ASTApi::CreateParameter(const Identifier &identifier, QualType type) {
  return context.make<ParameterDecl>(VariableDecl(identifier, std::move(type)));
}

ParenExpr* ASTApi::CreateParenExpr(Expr *expr) {
  ParenExpr *node = context.make<ParenExpr>(expr);
  node->getType() = expr->getType();
  return node;
}
BinaryOperator* ASTApi::CreateBinOp(BinaryOperator::Operator op, Expr *lhsR, Expr *rhsR) {
  BinaryOperator *node = context.make<BinaryOperator>(op, lhsR, rhsR);
  Expr *&lhs = node->getLhs(), *&rhs = node->getRhs();
  if (op == BinaryOperator::Assign) {
    node->getType() = lhs->getType().addQuals(QualType::Reference);
    if (lhs->getType().getType() != rhs->getType().getType())
      rhs = context.make<ImplicitCastExpr>(rhs, lhs->getType().modQuals());
  } else {
    auto trr = typePromotion(lhs->getType().getType()->getCanonicalType(), rhs->getType().getType()->getCanonicalType());
    if (!trr.first)
      throw(std::runtime_error("Invalid BinOp"));
    if (trr.second == 0)
      lhs = context.make<ImplicitCastExpr>(lhs, rhs->getType().modQuals());
    else if (trr.second == 1)
      rhs = context.make<ImplicitCastExpr>(rhs, lhs->getType().modQuals());
    switch (op) {
    case BinaryOperator::Equal:
    case BinaryOperator::NEQ:
    case BinaryOperator::LEQ:
    case BinaryOperator::GEQ:
    case BinaryOperator::Less:
    case BinaryOperator::Greater:
    case BinaryOperator::And:
    case BinaryOperator::Or:
      node->getType() = QualType(BoolType::get(&context.types()));
      break;
    case BinaryOperator::Plus:
    case BinaryOperator::Minus:
    case BinaryOperator::Multiply:
    case BinaryOperator::Divide:
      node->getType() = QualType(trr.first);
      break;
    default:
      break;
    }
  }
  return node;
}
}
