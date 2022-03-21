#ifndef __ANALYSIS_STMT_HH__
#define __ANALYSIS_STMT_HH__

#include "AnalysisInfo.hh"
#include <set>

namespace tlang {
class StmtAnalysis: public AnalysisInfo {
public:
  StmtAnalysis(Stmt &stmt) :
      stmt(&stmt) {
  }
  Stmt* operator*() const {
    return stmt;
  }
  Stmt* operator->() {
    return stmt;
  }
  void add(VariableDecl *var) {
    if (var)
      referenced_variables.insert(var);
  }
  std::set<VariableDecl*>& referencedVariables() {
    return referenced_variables;
  }
  const std::set<VariableDecl*>& referencedVariables() const {
    return referenced_variables;
  }
protected:
  std::set<VariableDecl*> referenced_variables;
  Stmt *stmt { };
};
class ParallelStmtAnalysis: public StmtAnalysis {
public:
  using StmtAnalysis::StmtAnalysis;
  void addShared(VariableDecl *var) {
    if (var)
      sharedVariables.insert(var);
  }
  void addPrivate(VariableDecl *var) {
    if (var)
      privateVariables.insert(var);
  }
  std::set<VariableDecl*> sharedVariables;
  std::set<VariableDecl*> privateVariables;
};
}
#endif
