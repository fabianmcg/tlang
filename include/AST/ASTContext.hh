#ifndef AST_ASTCONTEXT_HH
#define AST_ASTCONTEXT_HH

#include <cstdint>
#include <map>
#include <memory>
#include "Common.hh"
#include "TypeContext.hh"

namespace tlang {
struct ASTContext {
  ASTContext();
  ~ASTContext() = default;
  ASTContext(ASTContext&&) = default;
  ASTContext(const ASTContext&) = delete;
  ASTContext& operator=(ASTContext&&) = default;
  ASTContext& operator=(const ASTContext&) = delete;
  UniverseDecl* operator*() const;
  template <typename T, typename ...Args>
  T* make(Args &&...args) {
    return addNode(std::make_unique<T>(std::forward<Args>(args)...));
  }
  template <typename T, typename V>
  T* create(V &&val) {
    return addNode(std::make_unique<V>(std::forward<V>(val)));
  }
  void remove(ASTNode *node);
  void addModule(UnitDecl *unit, ModuleDecl *module);
  template <typename ...Args>
  UnitDecl* addUnit(const Identifier &name, Args &&...args) {
    auto unit = make<UnitDecl>(name, UnitContext(), std::forward<Args>(args)...);
    program->add(unit);
    return unit;
  }
  TypeContext& types();
  template <typename T, typename ...Args>
  QualType makeType(Args &&...args) {
    return QualType(T::get(&type_context, std::forward<Args>(args)...));
  }
  template <typename T, typename ...Args>
  QualType makeQualType(QualType::cvr_qualifiers quals, Args &&...args) {
    return QualType(quals, T::get(&type_context, std::forward<Args>(args)...));
  }
protected:
  std::map<uint64_t, std::unique_ptr<ASTNode>> nodes;
  TypeContext type_context;
  UniverseDecl *program { };
  UnitDecl *mainUnit { };
  template <typename T>
  T* addNode(std::unique_ptr<T> &&value) {
    if (value) {
      auto &node = nodes[reinterpret_cast<std::uintptr_t>(value.get())] = std::forward<std::unique_ptr<T>>(value);
      return static_cast<T*>(node.get());
    }
    return nullptr;
  }
};
struct ASTContextReference {
  ASTContextReference(ASTContext &context) :
      context(context) {
  }
  ASTContext &context;
};
}
#endif
