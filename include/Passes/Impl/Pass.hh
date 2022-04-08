#ifndef PASSES_IMPL_PASS_HH
#define PASSES_IMPL_PASS_HH

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/TypeName.h>
#include <iostream>

namespace tlang {
struct alignas(8) PassID {
};
namespace impl {
template <typename ASTUnit, typename ResultManager, typename RT, typename ...Args>
struct PassConcept {
  virtual ~PassConcept() = default;
  virtual RT run(ASTUnit &unit, ResultManager &manager, Args ...args) = 0;
  virtual llvm::StringRef name() const = 0;
};
template <typename ASTUnit, typename ResultManager, typename Pass, typename RT, typename ...Args>
struct PassModel: PassConcept<ASTUnit, ResultManager, RT, Args...> {
  PassModel(Pass runnable) :
      pass(std::move(runnable)) {
  }
  RT run(ASTUnit &unit, ResultManager &manager, Args ...args) override {
    std::cerr << "Running: " << name().str() << std::endl;
    return pass.run(unit, manager, args...);
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
