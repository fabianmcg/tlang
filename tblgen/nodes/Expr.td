#ifndef EXPR_TD
#define EXPR_TD
include "ASTNode.td"
include "Stmt.td"

def Expr : AbstractNode<ValueStmt, /* Abstract = */ 1, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
  ClassSection header = ClassSection<Header, Public, [{
    Expr(QualType &&type) : __children(std::forward<QualType>(type)) {
    }
  }]>;
}

def LiteralExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable value = Var<[{string}]>;
  ClassSection header = ClassSection<Header, Public, [{
    LiteralExpr(QualType &&type, std::string value) : Expr(std::forward<QualType>(type)), __value(value) {
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
  ClassSection header = ClassSection<Header, Public, [{
    ParenExpr(Expr *expr) : __children(expr) {
    }
  }]>;
}

def UnaryOperation : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable postfix = Var<[{bool}]>;
  Variable operator = Var<[{operator_kind}]>;
  Child expr = Child<Dynamic, [{Expr}]>;
  Enum operator_kind = Enum<[EM<"None", [{0}]>, EM<"Address", [{1}]>, EM<"Dereference", [{2}]>, EM<"Plus", [{3}]>, EM<"Minus", [{4}]>]>;
}

def BinaryOperation : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable operator = Var<[{OperatorKind}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    BinaryOperation(OperatorKind op, Expr *lhs, Expr *rhs) : Expr(), __operator(op), __children(lhs, rhs) {
    }
  }]>;
}

def DeclRefExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  Variable decl = Var<[{reference<Decl>}]>;
  ClassSection header = ClassSection<Header, Public, [{
    DeclRefExpr(const Identifier &identifier) : Expr(), __identifier(identifier) {
    }
    bool isResolved() const {
      return __decl;
    }
  }]>;
}

def MemberExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child owner = Child<Dynamic, [{Expr}]>;
  Child member = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    MemberExpr(Expr *lhs, Expr *rhs) : Expr(), __children(lhs, rhs) {
    }
  }]>;
}

def MemberRefExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable owner = Var<[{reference<Expr>}]>;
  Child member = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    MemberRefExpr(reference<Expr> owner, Expr *expr) : Expr(), __owner(owner), __children(expr) {
    }
  }]>;
}

def CallExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child callee = Child<Dynamic, [{Expr}]>;
  Child args = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    CallExpr(std::vector<Expr *> &&args) : __children(nullptr, std::forward<std::vector<Expr *>>(args)) {
    }
  }]>;
}

def ArrayExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child array = Child<Dynamic, [{Expr}]>;
  Child index = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ArrayExpr(std::vector<Expr *> &&index) : __children(nullptr, std::forward<std::vector<Expr *>>(index)) {
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
