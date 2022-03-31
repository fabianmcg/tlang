#ifndef AST_ASTCONTEXT_HH
#define AST_ASTCONTEXT_HH

#include "Common.hh"
#include "TypeContext.hh"
#include <Analysis/AnalysisContext.hh>
#include <cstdint>
#include <map>
#include <memory>

namespace tlang {
struct ASTContext {
  ASTContext() {
  }
  ~ASTContext() {
    module == nullptr;
  }
  ASTContext(ASTContext&&) = default;
  ASTContext(const ASTContext&) = delete;
  ASTContext& operator=(ASTContext&&) = default;
  ASTContext& operator=(const ASTContext&) = delete;
  ModuleDecl* operator*() const {
    return module;
  }
  template <typename T, typename ...Args>
  T* make(Args &&...args) {
    return add_node(std::make_unique<T>(std::forward<Args>(args)...));
  }
  template <typename T, typename V>
  T* create(V &&val) {
    return add_node(std::make_unique<V>(std::forward<V>(val)));
  }
  template <typename T>
  T* add_type(T &&value) {
    return add_node(std::make_unique<T>(std::forward<T>(value)));
  }
  template <typename T>
  void remove(T *node) {
    auto it = nodes.find(static_cast<uint64_t>(node));
    if (it != nodes.end())
      nodes.erase(it);
  }
  void add_module(ModuleDecl *module) {
    this->module = module;
  }
  AnalysisContext& analysisContext() {
    return analyis;
  }
  const AnalysisContext& analysisContext() const {
    return analyis;
  }
  TypeContext& types() {
    return type_context;
  }
protected:
  std::map<uint64_t, std::unique_ptr<ASTNode>> nodes;
  AnalysisContext analyis { };
  ModuleDecl *module { };
  TypeContext type_context;
  template <typename T>
  T* add_node(std::unique_ptr<T> &&value) {
    if (value) {
      auto &node = nodes[reinterpret_cast<std::uintptr_t>(value.get())] = std::forward<std::unique_ptr<T>>(value);
      return static_cast<T*>(node.get());
    }
    return nullptr;
  }
};
}
#endif
