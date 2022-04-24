#include <Parallel/AnalyzeRegions.hh>
#include <AST/Api.hh>
#include <algorithm>

namespace tlang {
namespace {
struct VariableDeclAnalysis: ASTVisitor<VariableDeclAnalysis, VisitorPattern::preOrder> {
  visit_t visitVariableDecl(VariableDecl *node) {
    variables->insert(node);
    return visit;
  }
  std::set<VariableDecl*> analyze(ASTNode *node) {
    std::set<VariableDecl*> variables;
    this->variables = &variables;
    dynamicTraverse(node);
    return variables;
  }
  std::set<VariableDecl*> *variables { };
};
struct VariableReferenceAnalysis: ASTVisitor<VariableReferenceAnalysis, VisitorPattern::preOrder> {
  visit_t visitDeclRefExpr(DeclRefExpr *node) {
    if (auto decl = node->getDecl().data())
      if (auto varDecl = dynamic_cast<VariableDecl*>(decl))
        referenced->insert(varDecl);
    return visit;
  }
  std::set<VariableDecl*> analyze(ASTNode *node) {
    std::set<VariableDecl*> referenced;
    this->referenced = &referenced;
    dynamicTraverse(node);
    return referenced;
  }
  std::set<VariableDecl*> *referenced { };
};
struct AnalysisVisitor: public ASTVisitor<AnalysisVisitor, VisitorPattern::prePostOrder> {
  AnalysisVisitor(ASTContext &context, std::set<ParallelConstruct*> &visited) :
      context(context), visited(visited) {
  }
  void addExpr(Expr *expr, std::set<VariableDecl*> &variables) {
    if (auto re = dyn_cast<DeclRefExpr>(expr))
      if (auto varDecl = dynamic_cast<VariableDecl*>(re))
        variables.insert(varDecl);
  }
  void addVariables(List<Expr*> &exprs, std::set<VariableDecl*> &variables) {
    for (auto expr : exprs)
      addExpr(expr, variables);
  }
  void addReduceVariables(ReduceStmt *stmt, std::set<VariableDecl*> &variables) {
    if (stmt)
      addVariables(stmt->getExprs(), variables);
  }
  void assignVariables(std::set<VariableDecl*> &variables, List<Expr*> &exprs) {
    ASTApi builder { context };
    exprs.clear();
    for (auto vd : variables) {
      exprs.push_back(builder.CreateDeclRefExpr(vd));
    }
  }
  template <typename T>
  void variableAnalysis(T *construct) {
    auto defined = VariableDeclAnalysis { }.analyze(construct);
    auto referenced = VariableReferenceAnalysis { }.analyze(construct);
    if (!construct->getParallelOptions())
      construct->getParallelOptions() = context.make<ParallelStmtOptions>();
    auto &firstPrivateV = construct->getParallelOptions()->getFirstPrivateVariables();
    auto &privateV = construct->getParallelOptions()->getPrivateVariables();
    auto &sharedV = construct->getParallelOptions()->getSharedVariables();
    std::set<VariableDecl*> fp, p, s, tmp1, tmp2;
    addVariables(sharedV, s);
    addVariables(privateV, p);
    addVariables(firstPrivateV, fp);
    for (auto rs : construct->getParallelOptions()->getReduceList())
      addReduceVariables(rs, s);
    std::set_difference(std::begin(referenced), std::end(referenced), std::begin(defined), std::end(defined),
        std::inserter(tmp1, std::begin(tmp1)));
    std::set_difference(std::begin(tmp1), std::end(tmp1), std::begin(s), std::end(s), std::inserter(tmp2, std::begin(tmp2)));
    tmp1.clear();
    std::set_difference(std::begin(tmp2), std::end(tmp2), std::begin(fp), std::end(fp), std::inserter(tmp1, std::begin(tmp1)));
    p.insert(tmp1.begin(), tmp1.end());
    assignVariables(s, sharedV);
    assignVariables(p, privateV);
    assignVariables(fp, firstPrivateV);
  }
  visit_t visitParallelStmt(ParallelStmt *stmt, VisitType kind) {
    if (visited.count(stmt))
      return skip;
    if (kind == postVisit)
      visited.insert(stmt);
    if (kind == preVisit)
      variableAnalysis(stmt);
    return visit;
  }
  visit_t visitLoopStmt(LoopStmt *stmt, VisitType kind) {
    if (visited.count(stmt))
      return skip;
    if (kind == postVisit)
      visited.insert(stmt);
    if (kind == preVisit)
      variableAnalysis(stmt);
    return visit;
  }
  ASTContext &context;
  std::set<ParallelConstruct*> &visited;
};
}
bool AnalyzeRegions::run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
  auto constructs = results.getResult<ParallelConstructDatabase>(CreateConstructDatabase::ID(), &decl);
  if (constructs) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Analyzing parallel constructs\n");
    for (auto c : constructs->contexts)
      AnalysisVisitor { CI.getContext(), visited }.traverseContextStmt(c.construct.node);
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green), "Finished analyzing parallel constructs\n");
  }
  return true;
}
}
