#ifndef PASSES_IMPL_PASSMANAGER_HH
#define PASSES_IMPL_PASSMANAGER_HH

#include <vector>
#include <AST/Visitors/EditVisitor.hh>
#include <Passes/Impl/Pass.hh>

namespace tlang::impl {
template <typename Node, typename ResultManager, typename RT, typename ...Args>
class PassManager: PassBase<PassManager<Node, ResultManager, RT, Args...>> {
protected:
  using pass_concept = PassConcept<Node, ResultManager, RT, Args...>;
  std::vector<std::unique_ptr<pass_concept>> passes;
public:
  PassManager() = default;
  RT run(Node &unit, AnyASTNodeRef nodeRef, ResultManager &manager, Args ...args) {
    for (auto &pass : passes) {
      if (pass)
        pass->run(unit, nodeRef, manager, args...);
    }
    return {};
  }
  template <typename Pass, std::enable_if_t<!std::is_same_v<Pass, PassManager>, int> = 0>
  void addPass(Pass &&pass) {
    using PassModelT = PassModel<Node, ResultManager, Pass, RT, Args...>;
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
template <typename Node, typename ResultManager, typename RT, typename ...Args>
struct PassManagerTraits<PassManager<Node, ResultManager, RT, Args...>> {
  using node = Node;
  using result_manager = ResultManager;
  using return_type = RT;
  using arguments = std::tuple<Args...>;
  using concept = PassConcept<Node, ResultManager, RT, Args...>;
  template <typename Pass>
  using model = PassModel<Node, ResultManager, Pass, RT, Args...>;
};

template <typename, typename, typename >
class PassManagerAdaptor;
template <typename PassNode, typename Pass, typename Node, typename ResultManager, typename RT, typename ...Args>
class PassManagerAdaptor<PassNode, Pass, PassManager<Node, ResultManager, RT, Args...>> : public PassBase<
    PassManagerAdaptor<PassNode, Pass, PassManager<Node, ResultManager, RT, Args...>>> {
public:
  struct Visitor: public tlang::EditVisitor<Visitor, VisitorPattern::preOrder> {
    using VisitStatus = typename tlang::EditVisitor<Visitor, VisitorPattern::preOrder>::VisitStatus;
    Visitor(Pass &pass, ResultManager &manager, std::tuple<Args...> &&args) :
        pass(pass), manager(manager), args(std::move(args)) {
    }
    VisitStatus visitASTNode(ASTNode *astNode, AnyASTNodeRef &nodeRef) {
      auto node = dyn_cast<PassNode>(astNode);
      if (!node)
        return VisitStatus::visit;
      if constexpr (sizeof...(Args)) {
        auto helper = [&](auto ...args) {
          pass.run(*node, nodeRef, manager, args...);
        };
        std::apply(helper, args);
      } else
        pass.run(*node, nodeRef, manager);
      return VisitStatus::visit;
    }
    Pass &pass;
    ResultManager &manager;
    std::tuple<Args...> args;
  };
  PassManagerAdaptor(Pass &&pass) :
      pass(std::move(pass)) {
  }
  RT run(Node &node, AnyASTNodeRef nodeRef, ResultManager &manager, Args ...args) {
    auto tuple = std::tuple<Args...> { std::forward<Args>(args)... };
    Visitor visitor { pass, manager, std::move(tuple) };
    visitor.dynamicTraverse(&node, nodeRef);
    return {};
  }
protected:
  Pass pass;
};

template <typename PassNode, typename PassManager, typename Pass>
PassManagerAdaptor<PassNode, Pass, PassManager> makePassAdaptor(Pass &&pass) {
  using PassModelT = typename PassManagerTraits<PassManager>::template model<Pass>;
  return PassManagerAdaptor<PassNode, Pass, PassManager>(std::move(pass));
}
}

#endif
