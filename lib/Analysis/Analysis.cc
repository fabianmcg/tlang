#include "Analysis/Analysis.hh"
#include <AST/RecursiveASTVisitor.hh>
#include <Analysis/Stmt.hh>

#include <algorithm>

namespace tlang {
namespace {
struct VariableDeclVisitor: RecursiveASTVisitor<VariableDeclVisitor> {
  VariableDeclVisitor(std::set<VariableDecl*> &variables) :
      variables(variables) {
  }
  visit_t visitVariableDecl(VariableDecl *node, bool isFirst) {
    variables.insert(node);
    return visit_value;
  }
  std::set<VariableDecl*> &variables;
};
struct VariableReferenceVisitor: RecursiveASTVisitor<VariableReferenceVisitor> {
  VariableReferenceVisitor(StmtAnalysis &analysis) :
      analysis(analysis) {
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    if (auto decl = node->getDecl().data())
      if (auto varDecl = dynamic_cast<VariableDecl*>(decl))
        analysis.add(varDecl);
    return visit_value;
  }
  void analyze() {
    dynamicTraverse(*analysis);
  }
  StmtAnalysis &analysis;
};
struct StmtVisitor: RecursiveASTVisitor<StmtVisitor> {
  StmtVisitor(AnalysisContext &context) :
      context(context) {
  }
  visit_t visitStmt(Stmt *node, bool isFirst) {
    auto kind = node->classOf();
    StmtAnalysis *analysis { };
    if (kind == NodeClass::ParallelStmt)
      analysis = context.make<ParallelStmtAnalysis>(node, *node);
    else
      analysis = context.make<StmtAnalysis>(node, *node);
    if (analysis)
      VariableReferenceVisitor { *analysis }.analyze();
    return visit_value;
  }
  AnalysisContext &context;
};
struct ParallelStmtHelperVisitor: RecursiveASTVisitor<ParallelStmtHelperVisitor> {
  ParallelStmtHelperVisitor(std::set<VariableDecl*> &variables) :
      variables(variables) {
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    if (auto decl = node->getDecl().data())
      if (auto varDecl = dynamic_cast<VariableDecl*>(decl))
        variables.insert(varDecl);
    return visit_value;
  }
  void analyze(std::vector<Expr*> &exprList) {
    for (auto expr : exprList)
      dynamicTraverse(expr);
  }
  std::set<VariableDecl*> &variables;
};
struct ParallelStmtVisitor: RecursiveASTVisitor<ParallelStmtVisitor> {
  ParallelStmtVisitor(AnalysisContext &context) :
      context(context) {
  }
  visit_t visitParallelStmt(ParallelStmt *node, bool isFirst) {
    if (ParallelStmtAnalysis *analysis = dynamic_cast<ParallelStmtAnalysis*>(context[node])) {
      auto &referenced = analysis->referencedVariables();
      auto &privateVariables = analysis->privateVariables;
      auto &sharedVariables = analysis->sharedVariables;
      auto &declaredPrivateV = node->getPrivatevariables();
      auto &declaredSharedV = node->getSharedvariables();
      ParallelStmtHelperVisitor { privateVariables }.analyze(declaredPrivateV);
      ParallelStmtHelperVisitor { sharedVariables }.analyze(declaredSharedV);
      std::set<VariableDecl*> tmp1, tmp2;
      VariableDeclVisitor { privateVariables }.dynamicTraverse(node->getStmt());
      std::set_difference(std::begin(referenced), std::end(referenced), std::begin(privateVariables), std::end(privateVariables),
          std::inserter(tmp1, std::begin(tmp1)));
      std::set_union(std::begin(tmp1), std::end(tmp1), std::begin(sharedVariables), std::end(sharedVariables),
          std::inserter(tmp2, std::begin(tmp2)));
      sharedVariables = tmp2;
      tmp1.clear();
      std::set_difference(std::begin(privateVariables), std::end(privateVariables), std::begin(sharedVariables), std::end(sharedVariables),
          std::inserter(tmp1, std::begin(tmp1)));
      privateVariables = tmp1;
    }
    return visit_value;
  }
  AnalysisContext &context;
};
}
void CodeAnalysis::run() {
  StmtVisitor { context.analysisContext() }.traverseModuleDecl(*context);
  ParallelStmtVisitor { context.analysisContext() }.traverseModuleDecl(*context);
}
}
