#ifndef __AST_TYPECONTEXT_HH__
#define __AST_TYPECONTEXT_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"
#include <cstdint>
#include <map>

namespace tlang {
class TypeContext {
public:
  TypeContext() {
    init_numeric();
  }
  Type* base_types(Type *type) const {
    switch (type->classof()) {
    case NodeClass::DependentType:
      return rm_const(dependent_type);
    case NodeClass::BoolType:
      return rm_const(bool_type);
    case NodeClass::StringType:
      return rm_const(string_type);
    case NodeClass::IntType: {
      IntType *it = static_cast<IntType*>(type);
      return rm_const(IntTypes[it->getSign()][it->getPrecision()]);
    }
    case NodeClass::FloatType: {
      FloatType *it = static_cast<FloatType*>(type);
      return rm_const(FloatTypes[it->getPrecision()]);
    }
    }
    return nullptr;
  }
  Type* static_types(Type *type) {
    switch (type->classof()) {
    case NodeClass::PtrType:
      return rm_const(dependent_type);
    }
    return nullptr;
  }
protected:
  DependentType dependent_type { };
  BoolType bool_type { };
  StringType string_type { };
  std::array<std::array<IntType, 5>, 2> IntTypes { };
  std::array<FloatType, 6> FloatTypes { };
  void init_numeric() {
    auto add_float = [&](FloatType::numeric_precision_t precision) {
      FloatTypes[precision] = FloatType { precision };
    };
    auto add_int = [&](IntType::numeric_precision_t precision, IntType::numeric_sign_t sign) {
      IntTypes[sign][precision] = IntType { precision, sign };
    };
    for (int p = FloatType::Default; p <= FloatType::P_128; ++p)
      add_float(static_cast<FloatType::numeric_precision_t>(p));
    for (int s = IntType::Signed; s <= IntType::Unsigned; ++s)
      for (int p = IntType::Default; p <= IntType::P_64; ++p)
        add_int(static_cast<IntType::numeric_precision_t>(p), static_cast<IntType::numeric_sign_t>(s));
  }
  template <typename T>
  std::remove_const_t<T>* rm_const(T &type) const {
    return const_cast<std::remove_const_t<T>*>(&type);
  }
};
}
#endif
