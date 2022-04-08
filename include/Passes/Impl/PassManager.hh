#ifndef PASSES_IMPL_PASSMANAGER_HH
#define PASSES_IMPL_PASSMANAGER_HH

#include <vector>
#include <Passes/Impl/Pass.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang::impl {
template <typename ASTNode, typename ResultsManager, typename RT, typename ...Args>
class PassManager: PassBase<PassManager<ASTNode, ResultsManager, RT, Args...>> {
protected:
  using pass_concept = PassConcept<ASTNode, ResultsManager, RT, Args...>;
  std::vector<std::unique_ptr<pass_concept>> passes;
public:
  PassManager() = default;
  RT run(ASTNode &unit, ResultsManager &manager, Args ...args) {
    for (auto &pass : passes) {
      if (pass)
        pass->run(unit, manager, args...);
    }
    return {};
  }
  template <typename Pass, std::enable_if_t<!std::is_same_v<Pass, PassManager>, int> = 0>
  void addPass(Pass &&pass) {
    using PassModelT = PassModel<ASTNode, ResultsManager, Pass, RT, Args...>;
    passes.push_back(std::unique_ptr<pass_concept>(new PassModelT(std::forward<Pass>(pass))));
  }
  template <typename Pass, std::enable_if_t<std::is_same_v<Pass, PassManager>, int> = 0>
  void addPass(Pass &&pass) {
    for (auto &p : pass)
      passes.push_back(std::move(p));
  }
  bool isEmpty() const {
    return passes.empty();
  }
  static bool isRequired() {
    return true;
  }
};
template <typename >
struct PassManagerTraits;
template <typename ASTUnit, typename ResultsManager, typename RT, typename ...Args>
struct PassManagerTraits<PassManager<ASTUnit, ResultsManager, RT, Args...>> {
  using type = PassConcept<ASTUnit, ResultsManager, RT, Args...>;
  template <typename Pass>
  using model = PassModel<ASTUnit, ResultsManager, Pass, RT, Args...>;
};

template <typename, typename, typename >
class PassManagerAdaptor;
template <typename PassASTUnit, typename Pass, typename ASTUnit, typename ResultsManager, typename RT, typename ...Args>
class PassManagerAdaptor<PassASTUnit, Pass, PassManager<ASTUnit, ResultsManager, RT, Args...>> : public PassBase<
    PassManagerAdaptor<PassASTUnit, Pass, PassManager<ASTUnit, ResultsManager, RT, Args...>>> {
public:
  struct Visitor: public tlang::ASTVisitor<Visitor, VisitorPattern::preOrder> {
    using VisitStatus = typename tlang::ASTVisitor<Visitor, VisitorPattern::preOrder>::VisitStatus;
    Visitor(Pass &pass, ResultsManager &manager, std::tuple<Args...> &&args) :
        pass(pass), manager(manager), args(std::move(args)) {
    }
    VisitStatus visitASTNode(ASTNode *astNode) {
      auto node = dyn_cast<PassASTUnit>(astNode);
      if (!node)
        return VisitStatus::visit;
      if constexpr (sizeof...(Args)) {
        auto helper = [&](auto ...args) {
          pass.run(*node, manager, args...);
        };
        std::apply(helper, args);
      } else
        pass.run(*node, manager);
      return VisitStatus::visit;
    }
    Pass &pass;
    ResultsManager &manager;
    std::tuple<Args...> args;
  };
  PassManagerAdaptor(Pass &&pass) :
      pass(std::move(pass)) {
  }
  RT run(ASTUnit &node, ResultsManager &manager, Args ...args) {
    auto tuple = std::tuple<Args...> { std::forward<Args>(args)... };
    Visitor visitor { pass, manager, std::move(tuple) };
    visitor.dynamicTraverse(&node);
    return {};
  }
protected:
  Pass pass;
};
template <typename PassASTUnit, typename PassManager, typename Pass>
PassManagerAdaptor<PassASTUnit, Pass, PassManager> makePassAdaptor(Pass &&pass) {
  using PassConcept = typename PassManagerTraits<PassManager>::type;
  using PassModelT = typename PassManagerTraits<PassManager>::model<Pass>;
  return PassManagerAdaptor<PassASTUnit, Pass, PassManager>(std::move(pass));
}
}

#endif
