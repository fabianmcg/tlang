#ifndef __AST_ATSCONTEXT_HH__
#define __AST_ASTCONTEXT_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"
#include <cstdint>
#include <map>

namespace _astnp_ {
class TypeContext {
public:
  TypeContext() {
    init();
  }
  Type* base_types(Type *type) {
    switch (type->classOf()) {
    case NodeClass::DependentType:
      return cast(dependent_type);
    case NodeClass::AutoType:
      return cast(auto_type);
    case NodeClass::VoidType:
      return cast(void_type);
    case NodeClass::BoolType:
      return cast(bool_type);
    case NodeClass::StringType:
      return cast(string_type);
    case NodeClass::IntType: {
      IntType *it = static_cast<IntType*>(type);
      return cast(IntTypes[it->getSign()][it->getPrecision()]);
    }
    case NodeClass::FloatType: {
      FloatType *it = static_cast<FloatType*>(type);
      return cast(FloatTypes[it->getPrecision()]);
    }
    }
    return nullptr;
  }
  Type* static_types(Type *type) {
    switch (type->classOf()) {
    case NodeClass::PtrType:
      return cast(dependent_type);
    }
    return nullptr;
  }
protected:
  const DependentType dependent_type { };
  const AutoType auto_type { };
  const VoidType void_type { };
  const BoolType bool_type { };
  const StringType string_type { };
  std::array<std::array<IntType, 5>, 2> IntTypes { };
  std::array<FloatType, 6> FloatTypes { };
  void init() {
    auto add_float = [&](FloatType::numeric_precision_t precision) {
      FloatTypes[precision] = FloatType { precision };
    };
    auto add_int = [&](IntType::numeric_precision_t precision, IntType::numeric_sign_t sign) {
      IntTypes[sign][precision] = IntType { precision, sign };
    };
    add_float(FloatType::Default);
    add_float(FloatType::P_8);
    add_float(FloatType::P_16);
    add_float(FloatType::P_32);
    add_float(FloatType::P_64);
    add_float(FloatType::P_128);
    add_int(IntType::Default, IntType::Signed);
    add_int(IntType::P_8, IntType::Signed);
    add_int(IntType::P_16, IntType::Signed);
    add_int(IntType::P_32, IntType::Signed);
    add_int(IntType::P_64, IntType::Signed);
    add_int(IntType::Default, IntType::Unsigned);
    add_int(IntType::P_8, IntType::Unsigned);
    add_int(IntType::P_16, IntType::Unsigned);
    add_int(IntType::P_32, IntType::Unsigned);
    add_int(IntType::P_64, IntType::Unsigned);
  }
  template <typename T>
  std::remove_const_t<T>* cast(T &type) {
    return const_cast<std::remove_const_t<T>*>(&type);
  }
};
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
  template <typename T, typename V>
  T* create(V &&value) {
    return add_node(std::make_unique<T>(std::forward<V>(value)));
  }
  template <typename T>
  T* add_type(T &&value) {
    return add_node(std::make_unique<T>(std::forward<T>(value)));
  }
  template <typename T>
  T* add_node(std::unique_ptr<T> &&value) {
    if (value) {
      auto &node = __nodes[reinterpret_cast<std::uintptr_t>(value.get())] = std::forward<std::unique_ptr<T> >(value);
      return static_cast<T*>(node.get());
    }
    return nullptr;
  }
  template <typename T>
  void remove_node(T *node) {
    auto it = __nodes.find(static_cast<uint64_t>(node));
    if (it != __nodes.end())
      __nodes.erase(it);
  }
  void set_module(ModuleDecl *module) {
    __module = module;
  }
protected:
  std::map<uint64_t, std::unique_ptr<ASTNode>> __nodes;
  ModuleDecl *__module { };
  TypeContext type_ctx { };
};
}
#endif
