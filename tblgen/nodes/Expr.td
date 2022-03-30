#ifndef EXPR_TD
#define EXPR_TD
include "ASTNode.td"
include "Stmt.td"

def ExprHeader : CodeSection<Header, [{
#include <AST/Stmt.hh>
#include <AST/Type.hh>
}]>;

def Expr : AbstractNode<ValueStmt, /* Abstract = */ 1, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
}

def LiteralExpr : AbstractNode<Expr, /* Abstract = */ 1, /* Implicit = */ 0> {
  Variable value = Var<[{std::string}]>;
  ClassSection header = ClassSection<Header, Public, [{
    LiteralExpr(QualType &&type, std::string value) : Expr(std::forward<QualType>(type)), value(value) {
    }
  }]>;
}

def BooleanLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def IntegerLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    IntegerLiteral(QualType &&type, std::string value) : LiteralExpr(std::forward<QualType>(type), value) {
    }
  }]>;
}

def FloatLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    FloatLiteral(QualType &&type, std::string value) : LiteralExpr(std::forward<QualType>(type), value) {
    }
  }]>;
}

def ComplexLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def StringLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    StringLiteral(QualType &&type, std::string value) : LiteralExpr(std::forward<QualType>(type), value) {
    }
  }]>;
}

def ThisExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ParenExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

def UnaryOperator : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Operator = Enum<[EM<"None", [{0}]>, EM<"Address", [{1}]>, EM<"Dereference", [{2}]>, EM<"Plus", [{3}]>, EM<"Minus", [{4}]>]>;
  Variable postfix = Var<[{bool}]>;
  Variable op = EVar<[{Operator}]>;
  Child expr = Child<Dynamic, [{Expr}]>;
}

def BinaryOperator : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Operator = Enum<[EM<"None">,
    EM<"And">,
    EM<"Or">,
    EM<"Equal">,
    EM<"NEQ">,
    EM<"LEQ">,
    EM<"GEQ">,
    EM<"LShift">,
    EM<"RShift">,
    EM<"Pow">,
    EM<"Increment">,
    EM<"Decrement">,
    EM<"Ellipsis">,
    EM<"Not">,
    EM<"Band">,
    EM<"Bor">,
    EM<"Less">,
    EM<"Greater">,
    EM<"Assign">,
    EM<"Plus">,
    EM<"Minus">,
    EM<"Multiply">,
    EM<"Divide">,
    EM<"Modulo">,
    EM<"At">,
    EM<"Dot">,
    EM<"Comma">,
    EM<"Colon">,
    EM<"Semicolon">
  ]>;
  Variable op = EVar<[{Operator}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
}

def DeclRefExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  Variable decl = Var<[{reference<Decl>}]>;
  ClassSection header = ClassSection<Header, Public, [{
    DeclRefExpr(const Identifier &identifier) : Expr(), identifier(identifier) {
    }
    bool isResolved() const {
      return decl;
    }
  }]>;
}

def MemberExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child owner = Child<Dynamic, [{Expr}]>;
  Child member = Child<Dynamic, [{Expr}]>;
}

def MemberRefExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable owner = Var<[{reference<Expr>}]>;
  Child member = Child<Dynamic, [{Expr}]>;
}

def CallExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child callee = Child<Dynamic, [{Expr}]>;
  Child args = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    CallExpr(std::vector<Expr *> &&args) : children(nullptr, std::forward<std::vector<Expr *>>(args)) {
    }
  }]>;
}

def ArrayExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child array = Child<Dynamic, [{Expr}]>;
  Child index = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ArrayExpr(std::vector<Expr *> &&index) : children(nullptr, std::forward<std::vector<Expr *>>(index)) {
    }
  }]>;
}

def CastExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

def ImplicitCastExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

def RangeExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child start = Child<Dynamic, [{Expr}]>;
  Child step = Child<Dynamic, [{Expr}]>;
  Child stop = Child<Dynamic, [{Expr}]>;
}

def AllocateExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

#endif
