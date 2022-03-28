#ifndef AST_NODE_TD
#define AST_NODE_TD

include "Infra/Variable.td"
include "Infra/Children.td"
include "Infra/CodeSection.td"
include "Infra/Specifier.td"
include "Infra/Enum.td"

class AbstractNode<AbstractNode Base = ?, bit Abstract = 0, bit Implicit = 0, bit Top = 0,
				   DeclSpecifier Copyable = Default, DeclSpecifier Moveable = Default, DeclSpecifier DefaultDestructor = Default, DeclSpecifier DefaultConstructor = Default> {
  AbstractNode base = Base;
  bit abstract = Abstract;
  bit implicit = Implicit;
  bit top = Top;
  DeclSpecifier copyable = Copyable;
  DeclSpecifier moveable = Moveable;
  DeclSpecifier defaultDestructor = DefaultDestructor;
  DeclSpecifier defaultConstructor = DefaultConstructor;
  bit visit = 1;
  bit implement = 1;
  bit emittable = !not(abstract);
  code irType = ?;
}

def ASTNode : AbstractNode<?, 1, 0, 1> {
  let implement = 0;
}

#endif
