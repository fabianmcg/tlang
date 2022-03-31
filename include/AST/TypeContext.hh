#ifndef AST_TYPECONTEXT_HH
#define AST_TYPECONTEXT_HH

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include "Decl.hh"
#include "Type.hh"

namespace tlang {
class TypeContext {
public:
  TypeContext() {
    init();
  }
  UnresolvedType* getUnresolvedType(const Identifier &id) {
    return addType(UnresolvedType(id));
  }
  AddressType* getAddressType() {
    return &addres_type;
  }
  BoolType* getBoolType() {
    return &bool_type;
  }
  StringType* getStringType() {
    return &string_type;
  }
  IntType* getIntType(IntType::numeric_precision precision, IntType::numeric_sign sign) {
    if (precision == IntType::Default)
      return &(int_types[sign][IntType::P_32]);
    return &(int_types[sign][precision]);
  }
  FloatType* getFloatType(FloatType::numeric_precision precision) {
    if (precision == FloatType::Default)
      return &(float_types[FloatType::P_32]);
    return &(float_types[precision]);
  }
  VariadicType* getVariadicType(Type *underlying) {
    return addType(VariadicType(underlying));
  }
  PtrType* getPtrType(Type *underlying) {
    if (underlying) {
      auto &type = ptrTypes[underlying];
      if (type)
        return type;
      if (!underlying->getCanonicalType())
        throw(std::runtime_error("Invalid underlying type"));
      auto &canonicalType = ptrTypes[underlying->getCanonicalType().data()];
      if (!canonicalType) {
        canonicalType = addType(PtrType(underlying->getCanonicalType().data()));
        canonicalType->getCanonicalType() = canonicalType;
      }
      type = addType(PtrType(underlying));
      type->getCanonicalType() = canonicalType;
      return type;
    }
    return nullptr;
  }
  ArrayType* getArrayType(Type *underlying, List<Expr*> &&layout) {
    return addType(ArrayType(underlying, std::forward<List<Expr*>>(layout)));
  }
  StructType* getStructType(StructDecl *decl) {
    if (decl) {
      auto &type = decl_types[decl];
      if (type)
        return static_cast<StructType*>(type.get());
      auto st = std::make_unique<StructType>();
      st.get()->getDecl() = decl;
      st->getCanonicalType() = st.get();
      type = std::move(st);
      return static_cast<StructType*>(type.get());
    }
    return nullptr;
  }
  EnumType* getEnumType(EnumDecl *decl) {
    if (decl) {
      auto &type = decl_types[decl];
      if (type)
        return static_cast<EnumType*>(type.get());
      auto st = std::make_unique<EnumType>();
      st->getDecl() = decl;
      st->getCanonicalType() = st.get();
      type = std::move(st);
      return static_cast<EnumType*>(type.get());
    }
    return nullptr;
  }
protected:
  AddressType addres_type { };
  BoolType bool_type { };
  StringType string_type { };
  std::array<std::array<IntType, 5>, 2> int_types { };
  std::array<FloatType, 6> float_types { };
  std::map<Decl*, std::unique_ptr<Type>> decl_types;
  std::map<Type*, std::unique_ptr<Type>> types;
  std::map<Type*, PtrType*> ptrTypes;
  template <typename T>
  T* addType(T &&type) {
    auto t = std::make_unique<T>(std::move(type));
    auto address = t.get();
    types[address] = std::move(t);
    address->getCanonicalType() = address;
    return address;
  }
  void init() {
    addres_type.getCanonicalType() = &addres_type;
    bool_type.getCanonicalType() = &bool_type;
    string_type.getCanonicalType() = &string_type;
    auto add_float = [&](FloatType::numeric_precision precision) {
      float_types[precision] = FloatType { precision };
      float_types[precision].getCanonicalType() = &(float_types[precision]);
    };
    auto add_int = [&](IntType::numeric_precision precision, IntType::numeric_sign sign) {
      int_types[sign][precision] = IntType { precision, sign };
      int_types[sign][precision].getCanonicalType() = &(int_types[sign][precision]);
    };
    for (int p = FloatType::P_8; p <= FloatType::P_128; ++p)
      add_float(static_cast<FloatType::numeric_precision>(p));
    for (int s = IntType::Signed; s <= IntType::Unsigned; ++s)
      for (int p = IntType::P_8; p <= IntType::P_64; ++p)
        add_int(static_cast<IntType::numeric_precision>(p), static_cast<IntType::numeric_sign>(s));
  }
};
}
#endif
