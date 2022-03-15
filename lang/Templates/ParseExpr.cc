parse_result<Expr, return_kind::Dynamic> ParseBinOpRHS(int exprPrecedence, parse_result<Expr, return_kind::Dynamic> lhs) {
  using binary_t = parse_result<BinaryOperation, return_kind::Dynamic>;
  using member_t = parse_result<MemberExpr, return_kind::Dynamic>;
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
    if (bop == OperatorKind::Dot)  {
      // auto node = make_node<MemberRefExpr>(*lhs, *rhs);
      lhs = make<member_t>(*lhs, *rhs);
      // node->getSourceRange() = SourceRange(lhsRange.begin, rhsRange.end);
    }
    else
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
