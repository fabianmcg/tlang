#ifndef PASSES_IMPL_PASS_HH
#define PASSES_IMPL_PASS_HH

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/TypeName.h>
#include <iostream>
#include <AST/AnyNode.hh>

namespace tlang {
struct alignas(8) PassID {
};
namespace impl {
template <typename ASTUnit, typename ResultManager, typename RT, typename ...Args>
struct PassConcept {
  virtual ~PassConcept() = default;
  virtual RT run(ASTUnit &unit, AnyASTNodeRef nodeRef, ResultManager &manager, Args ...args) = 0;
  virtual llvm::StringRef name() const = 0;
};
template <typename ASTUnit, typename ResultManager, typename Pass, typename RT, typename ...Args>
struct PassModel: PassConcept<ASTUnit, ResultManager, RT, Args...> {
  PassModel(Pass runnable) :
      pass(std::move(runnable)) {
  }
  RT run(ASTUnit &unit, AnyASTNodeRef nodeRef, ResultManager &manager, Args ...args) override {
    return pass.run(unit, nodeRef, manager, args...);
  }
  llvm::StringRef name() const override {
    return Pass::name();
  }
  Pass pass;
};
}
template <typename Derived>
struct PassBase {
  static llvm::StringRef name() {
    static_assert(std::is_base_of<PassBase, Derived>::value);
    llvm::StringRef name = llvm::getTypeName<Derived>();
    name.consume_front("llvm::");
    return name;
  }
};
}

#endif
