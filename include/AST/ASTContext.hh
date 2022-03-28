#ifndef __AST_ASTCONTEXT_HH__
#define __AST_ASTCONTEXT_HH__

#include "Common.hh"
//#include "TypeContext.hh"
#include <Analysis/AnalysisContext.hh>
#include <cstdint>
#include <map>
#include <memory>

namespace tlang {
struct ASTContext {
  ASTContext() {
  }
  ~ASTContext() {
    __module == nullptr;
  }
  ASTContext(ASTContext&&) = default;
  ASTContext(const ASTContext&) = delete;
  ASTContext& operator=(ASTContext&&) = default;
  ASTContext& operator=(const ASTContext&) = delete;
  ModuleDecl* operator*() const {
    return __module;
  }
  template <typename T, typename ...Args>
  T* make(Args &&...args) {
    return add_node(std::make_unique<T>(std::forward<Args>(args)...));
  }
  template <typename T>
  T* add_type(T &&value) {
    return add_node(std::make_unique<T>(std::forward<T>(value)));
  }
  template <typename T>
  void remove(T *node) {
    auto it = __nodes.find(static_cast<uint64_t>(node));
    if (it != __nodes.end())
      __nodes.erase(it);
  }
  void add_module(ModuleDecl *module) {
    __module = module;
  }
  AnalysisContext& analysisContext() {
    return __analyis;
  }
  const AnalysisContext& analysisContext() const {
    return __analyis;
  }
protected:
  std::map<uint64_t, std::unique_ptr<ASTNode>> __nodes;
  AnalysisContext __analyis { };
  ModuleDecl *__module { };
  template <typename T>
  T* add_node(std::unique_ptr<T> &&value) {
    if (value) {
      auto &node = __nodes[reinterpret_cast<std::uintptr_t>(value.get())] = std::forward<std::unique_ptr<T>>(value);
      return static_cast<T*>(node.get());
    }
    return nullptr;
  }
};
}
#endif
