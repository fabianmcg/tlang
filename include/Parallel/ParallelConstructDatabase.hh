#ifndef PARALLEL_PARALLELCONSTRUCTDATABASE_HH
#define PARALLEL_PARALLELCONSTRUCTDATABASE_HH

#include <AST/AnyNode.hh>
#include <Passes/ResultManager.hh>

namespace tlang {
template <typename T>
struct ConstructData {
  using type = T;
  ConstructData(const ASTData<T> &node, FunctorDecl *functor) :
      construct(node), functor(functor) {
  }
  ConstructData(T *node, AnyASTNodeRef ref, FunctorDecl *functor) :
      construct( { node, ref }), functor(functor) {
  }
  ASTData<T>& operator*() {
    return construct;
  }
  ASTData<T>* operator->() {
    return &construct;
  }
  ASTData<T> construct;
  FunctorDecl *functor { };
};
struct ParallelConstructDatabase: public ResultConcept {
  std::list<ConstructData<ContextStmt>> contexts;
  std::list<ConstructData<ParallelStmt>> hostRegions;
  std::list<ConstructData<ParallelStmt>> deviceRegions;
  std::list<ConstructData<ParallelStmt>> sequentialRegions;
  bool contextsQ() const {
    return contexts.size();
  }
  bool hostQ() const {
    return hostRegions.size();
  }
  bool deviceQ() const {
    return deviceRegions.size();
  }
  bool sequentialQ() const {
    return sequentialRegions.size();
  }
  bool empty() const {
    return !contextsQ() && !hostQ() && !deviceQ() && !sequentialQ();
  }
};
}

#endif
