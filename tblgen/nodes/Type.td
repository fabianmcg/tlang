#ifndef TYPE_TD
#define TYPE_TD
include "ASTNode.td"

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
}

def DependentType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def UnresolvedType : AbstractNode<DependentType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
}

def BuiltinType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def AddressType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def BoolType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def StringType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def IntType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable precision = EVar<[{numeric_precision}]>;
  Variable sign = EVar<[{numeric_sign}]>;
  Enum numeric_precision = Enum<[EM<"Default">, EM<"P_8">, EM<"P_16">, EM<"P_32">, EM<"P_64">]>;
  Enum numeric_sign = Enum<[EM<"Signed">, EM<"Unsigned">]>;
}

def FloatType : AbstractNode<BuiltinType, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable precision = EVar<[{numeric_precision}]>;
  Enum numeric_precision = Enum<[EM<"Default">, EM<"P_8">, EM<"P_16">, EM<"P_32">, EM<"P_64">, EM<"P_128">]>;
}

def VariadicType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
}

def PtrType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
}

def ArrayType : AbstractNode<Type, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child underlying = Child<Dynamic, [{Type}]>;
  Child layout = Child<DynamicList, [{Expr}]>;
}

def DefinedType : AbstractNode<Type, /* Abstract = */ 1, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  Variable decl = Var<[{reference<NamedDecl>}]>;
}

def TagType : AbstractNode<DefinedType, /* Abstract = */ 1, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    TagType(const Identifier &identifier, reference<NamedDecl> decl = {}) : DefinedType(identifier, decl) {
    }
  }]>;
}

def StructType : AbstractNode<TagType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    StructType(const Identifier &identifier, reference<NamedDecl> decl = {}) : TagType(identifier, decl) {
    }
  }]>;
}

def EnumType : AbstractNode<TagType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    EnumType(const Identifier &identifier, reference<NamedDecl> decl = {}) : TagType(identifier, decl) {
    }
  }]>;
}

def TypedefType : AbstractNode<DefinedType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    TypedefType(const Identifier &identifier, reference<NamedDecl> decl = {}) : DefinedType(identifier, decl) {
    }
  }]>;
}

def FunctionType : AbstractNode<DefinedType, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    FunctionType(const Identifier &identifier, reference<NamedDecl> decl = {}) : DefinedType(identifier, decl) {
    }
  }]>;
}

#endif
