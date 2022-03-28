#ifndef ATTR_TD
#define ATTR_TD
include "ASTNode.td"

def AttrHeader : CodeSection<Header, [{
#include <AST/Type.hh>
}]>;

def Attr : AbstractNode<ASTNode, /* Abstract = */ 1, /* Implicit = */ 0, /* Top = */ 1> {
}

def AttrList : AbstractNode<ASTNode, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child attributes = Child<DynamicList, [{Attr}]>;
}

def NamedAttr : AbstractNode<Attr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  Child attr = Child<Dynamic, [{Attr}]>;
}

def TypeAttr : AbstractNode<Attr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
}

def ExprAttr : AbstractNode<Attr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

def LiteralAttr : AbstractNode<ExprAttr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def DeclAttr : AbstractNode<Attr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child decl = Child<Dynamic, [{Decl}]>;
}

#endif
