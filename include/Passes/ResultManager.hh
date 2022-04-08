#ifndef PASSES_RESULTMANAGER_HH
#define PASSES_RESULTMANAGER_HH

#include <list>
#include <llvm/ADT/DenseMap.h>
#include <Passes/Impl/Pass.hh>

namespace tlang {
class ASTNode;
}
namespace tlang {
struct ResultConcept {
};
class ResultManager {
private:
  using result_concept = ResultConcept;
  using result_list = std::list<std::pair<PassID *, std::unique_ptr<result_concept>>>;
  using result_map = llvm::DenseMap<ASTNode *, result_list>;
  using result_table = llvm::DenseMap<std::pair<PassID *, ASTNode *>, typename result_list::iterator>;
public:
  template <typename Result>
  Result* getResult(PassID *key, ASTNode *node) {
    auto it = lut.find( { key, node });
    if (it == lut.end())
      return nullptr;
    return static_cast<Result*>(it->second->second.get());
  }
  template <typename Result>
  void addResult(PassID *key, ASTNode *node, Result *result) {
    auto &node_results = results[node];
    auto it = node_results.insert(node_results.end(), { key, std::unique_ptr<result_concept>(result) });
    lut[ { key, node }] = it;
  }
  void eraseResult(PassID *key, ASTNode *node) {
    auto it = lut.find( { key, node });
    if (it != lut.end()) {
      auto ot = it->second;
      auto &node_results = results[node];
      node_results.erase(ot);
      lut.erase(it);
    }
  }
protected:
  result_map results;
  result_table lut;
};
}

#endif
