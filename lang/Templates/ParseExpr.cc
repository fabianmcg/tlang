parse_result<Expr, return_kind::Dynamic> ParseBinOpRHS(int exprPrecedence, parse_result<Expr, return_kind::Dynamic> lhs) {
  using binary_t = parse_result<BinaryOperator, return_kind::Dynamic>;
  using member_t = parse_result<MemberExpr, return_kind::Dynamic>;
  using ternary_t = parse_result<TernaryOperator, return_kind::Dynamic>;
  using memberCall_t = parse_result<MemberCallExpr, return_kind::Dynamic>;
  while (true) {
    auto bop = tokToOperator(peek().kind);
    int token_precedence = operatorPrecedence(bop);
    if (token_precedence < exprPrecedence)
      return lhs;
    match(peek().kind, "BinOpRHS");
    auto rhs = ParseTopExpr();
    if (!rhs)
      syntaxError("ParseBinOpRHS");
    auto nbop = tokToOperator(peek().kind);
    int next_precedence = operatorPrecedence(nbop);
    if (token_precedence < next_precedence) {
      rhs = ParseBinOpRHS(token_precedence + 1, std::move(rhs));
      if (!rhs)
        syntaxError("ParseBinOpRHS");
    }
    auto lhsRange = lhs.range();
    auto rhsRange = rhs.range();
    if (bop == BinaryOperator::Dot)  {
      if (auto call = dyn_cast<CallExpr>(*rhs)) {
          auto member = make<member_t>(call->getCallee(), *lhs);
          lhs = make<memberCall_t>(std::move(*call));
          dyn_cast<MemberCallExpr>((*lhs))->getCallee() = *member;
          ctx.remove(call);
      }
      else
        lhs = make<member_t>(*rhs, *lhs);
    } else if ( bop == BinaryOperator::Ternary) {
      match(token_kind::Colon, "BinOpRHS");
      auto tmp = ParseTopExpr();
      lhs = make<ternary_t>(*lhs, *rhs, *ParseBinOpRHS(0, std::move(tmp)));
    } else
      lhs = make<binary_t>(bop, *lhs, *rhs);
    lhs.range(lhsRange.begin, rhsRange.end);
  }
}
parse_result<Expr, return_kind::Dynamic> ParseExpr() {
  auto lhs = ParseTopExpr();
  if (!lhs)
    syntaxError("ParseExpr");
  return ParseBinOpRHS(0, std::move(lhs));
}
