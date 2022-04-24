#include <AST/Api.hh>

namespace tlang {
CompoundStmt* ASTApi::CreateCompoundStmt(List<Stmt*> &&stmts) {
  return context.make<CompoundStmt>(std::move(stmts));
}
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

VariableDecl* ASTApi::CreateVariable(const Identifier &identifier, QualType type, Expr *init, VariableDecl::StorageKind storage) {
  VariableDecl var(storage, init);
  var.getIdentifier() = identifier;
  var.getType() = type;
  return context.make<VariableDecl>(var);
}
ParameterDecl* ASTApi::CreateParameter(const Identifier &identifier, QualType type) {
  return context.make<ParameterDecl>(VariableDecl(identifier, std::move(type)));
}
DeclRefExpr* ASTApi::CreateDeclRefExpr(ValueDecl *decl) {
  assert(decl);
  DeclRefExpr expr(decl->getIdentifier(), decl);
  expr.getType() = decl->getType().addQuals(QualType::Reference);
  return context.make<DeclRefExpr>(expr);
}
CallExpr* ASTApi::CreateCallExpr(DeclRefExpr *callee, List<Expr*> &&args) {
  assert(callee);
  CallExpr expr(callee, std::move(args));
  if (auto ft = dyn_cast<FunctionType>(callee->getType().getType()))
    expr.getType() = ft->getReturnType();
  return context.make<CallExpr>(expr);
}
ParenExpr* ASTApi::CreateParenExpr(Expr *expr) {
  ParenExpr *node = context.make<ParenExpr>(expr);
  node->getType() = expr->getType();
  return node;
}
BooleanLiteral* ASTApi::CreateLiteral(bool value) {
  return context.make<BooleanLiteral>(context.types(), value);
}
IntegerLiteral* ASTApi::CreateLiteral(int64_t value, IntType::numeric_precision precision) {
  return context.make<IntegerLiteral>(context.types(), value, precision);
}
UIntegerLiteral* ASTApi::CreateLiteral(uint64_t value, IntType::numeric_precision precision) {
  return context.make<UIntegerLiteral>(context.types(), value, precision);
}
FloatLiteral* ASTApi::CreateLiteral(double value, FloatType::numeric_precision precision) {
  return context.make<FloatLiteral>(context.types(), value, precision);
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
UnaryOperator* ASTApi::CreateUnOp(UnaryOperator::Operator op, Expr *expr) {
  UnaryOperator *node = context.make<UnaryOperator>(false, op, expr);
  switch (op) {
  case UnaryOperator::Dereference:
    if (auto pt = dyn_cast<PtrType>(expr->getType().getType()))
      node->getType() = QualType(QualType::Reference, expr->getType().getAddressSpace(), pt->getUnderlying());
    break;
  case UnaryOperator::Address:
    node->getType() = QualType(QualType::None, 0, PtrType::get(&context.types(), expr->getType().getType()));
    break;
  default:
    break;
  }
  return node;
}
CastExpr* ASTApi::CreateCast(Expr *expr, QualType type) {
  return context.make<CastExpr>(expr, std::move(type));
}
DeclStmt* ASTApi::CreateDeclStmt(List<VariableDecl*> &&variables) {
  return context.make<DeclStmt>(std::move(variables));
}
Stmt* ASTApi::PrependStmt(CompoundStmt *cs, Stmt *stmt) {
  assert(cs);
  if (stmt)
    cs->addStmt(stmt);
  return stmt;
}
Stmt* ASTApi::AppendStmt(CompoundStmt *cs, Stmt *stmt) {
  assert(cs);
  if (stmt)
    cs->addStmt(stmt);
  return stmt;
}
}
