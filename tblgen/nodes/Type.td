#ifndef TYPE_TD
#define TYPE_TD
include "ASTNode.td"

def TypeHeader : CodeSection<Header, [{
namespace tlang {
class TypeContext;
}
}]>;

def TypeSource : CodeSection<Source, [{
#include <AST/ASTContext.hh>
}]>;

def ProtoType : AbstractNode<ASTNode, /* Abstract = */ 1, /* Implicit = */ 0, /* Top = */ 1> {
  code irType = [{llvm::Type*}];
}

def QualType : AbstractNode<ProtoType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable qualifiers = EVar<[{cvr_qualifiers}]>;
  Child type = Child<Dynamic, [{Type}]>;
  Enum cvr_qualifiers = Enum<[EM<"None", [{0}]>, EM<"Reference", [{1}]>, EM<"Const", [{2}]>]>;
  ClassSection header = ClassSection<Header, Public, [{
    QualType(Type *type, cvr_qualifiers qualifiers = None) : qualifiers(qualifiers), children(type) {
    }
  }]>;
}

def Type : AbstractNode<ProtoType, /* Abstract = */ 1, /* Implicit = */ 0> {
  Variable canonicalType = Var<[{reference<Type>}]>;
}

def DependentType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def UnresolvedType : AbstractNode<DependentType, /* Abstract = */ 0, /* Implicit = */ 0> {
  let defaultConstructor = UserDefined;
  Variable identifier = Var<[{Identifier}]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static UnresolvedType* get(TypeContext *context, const Identifier& id);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    UnresolvedType* UnresolvedType::get(TypeContext *context, const Identifier& id) {
      return context->getUnresolvedType(id);
    }
  }]>;
}

def BuiltinType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def AddressType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static AddressType* get(TypeContext *context);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    AddressType* AddressType::get(TypeContext *context) {
      return context->getAddressType();
    }
  }]>;
}

def BoolType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static BoolType* get(TypeContext *context);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    BoolType* BoolType::get(TypeContext *context) {
      return context->getBoolType();
    }
  }]>;
}

def StringType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static StringType* get(TypeContext *context);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    StringType* StringType::get(TypeContext *context) {
      return context->getStringType();
    }
  }]>;
}

def IntType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable precision = EVar<[{numeric_precision}]>;
  Variable sign = EVar<[{numeric_sign}]>;
  Enum numeric_precision = Enum<[EM<"Default">, EM<"P_8">, EM<"P_16">, EM<"P_32">, EM<"P_64">]>;
  Enum numeric_sign = Enum<[EM<"Signed">, EM<"Unsigned">]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static IntType* get(TypeContext *context, numeric_precision precision, numeric_sign sign);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    IntType* IntType::get(TypeContext *context, numeric_precision precision, numeric_sign sign) {
      return context->getIntType(precision, sign);
    }
  }]>;
}

def FloatType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable precision = EVar<[{numeric_precision}]>;
  Enum numeric_precision = Enum<[EM<"Default">, EM<"P_8">, EM<"P_16">, EM<"P_32">, EM<"P_64">, EM<"P_128">]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static FloatType* get(TypeContext *context, numeric_precision precision);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    FloatType* FloatType::get(TypeContext *context, numeric_precision precision) {
      return context->getFloatType(precision);
    }
  }]>;
}

def VariadicType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static VariadicType* get(TypeContext *context, Type* underlying);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    VariadicType* VariadicType::get(TypeContext *context, Type* underlying) {
      return context->getVariadicType(underlying);
    }
  }]>;
}

def PtrType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static PtrType* get(TypeContext *context, Type* underlying);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    PtrType* PtrType::get(TypeContext *context, Type* underlying) {
      return context->getPtrType(underlying);
    }
  }]>;
}

def ArrayType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
  Child layout = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static ArrayType* get(TypeContext *context, Type* underlying, List<Expr*>&& layout);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    ArrayType* ArrayType::get(TypeContext *context, Type* underlying, List<Expr*>&& layout) {
      return context->getArrayType(underlying, std::forward<List<Expr*>>(layout));
    }
  }]>;
}

def DefinedType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
  Variable decl = Var<[{reference<NamedDecl>}]>;
}

def TagType : AbstractNode<DefinedType, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def StructType : AbstractNode<TagType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static StructType* get(TypeContext *context, StructDecl* decl);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    StructType* StructType::get(TypeContext *context, StructDecl* decl) {
      return context->getStructType(decl);
    }
  }]>;
}

def EnumType : AbstractNode<TagType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    friend class TypeContext;
    static EnumType* get(TypeContext *context, EnumDecl* decl);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    EnumType* EnumType::get(TypeContext *context, EnumDecl* decl) {
      return context->getEnumType(decl);
    }
  }]>;
}

def TypedefType : AbstractNode<DefinedType, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def FunctionType : AbstractNode<DefinedType, /* Abstract = */ 0, /* Implicit = */ 0> {
}

#endif
