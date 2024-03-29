#ifndef EXPR_TD
#define EXPR_TD
include "ASTNode.td"
include "Stmt.td"

def ExprHeader : CodeSection<Header, [{
#include <memory>
#include <ADT/Literal.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
}]>;

def Expr : AbstractNode<ValueStmt, /* Abstract = */ 1, /* Implicit = */ 0> {
  Child type = Child<Static, [{QualType}]>;
}

def LiteralExpr : AbstractNode<Expr, /* Abstract = */ 1, /* Implicit = */ 0> {
  Variable value = Var<[{std::shared_ptr<LiteralValue>}]>;
}

def BooleanLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    BooleanLiteral(TypeContext& context, bool value);
    bool getValue() const;
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    BooleanLiteral::BooleanLiteral(TypeContext& context, bool value): 
      BooleanLiteral(0) {
      this->value = std::make_shared<BoolValue>(value);
      getType() = QualType(BoolType::get(&context));
    }
    bool BooleanLiteral::getValue() const {
        if (value)
        return value->template getAs<BoolValue>().getValue();
        return false;
    }
  }]>;
}

def IntegerLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    IntegerLiteral(TypeContext& context, const std::string& value, IntType::numeric_precision presicion);
    IntegerLiteral(TypeContext& context, int64_t value, IntType::numeric_precision presicion);
    int64_t& getValue();
    int64_t getValue() const;
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    IntegerLiteral::IntegerLiteral(TypeContext& context, const std::string& value, IntType::numeric_precision presicion): 
      IntegerLiteral(0) {
      this->value = std::make_shared<IntValue>(std::stoll(value));
      getType() = QualType(IntType::get(&context, presicion, IntType::Signed));
    }
    IntegerLiteral::IntegerLiteral(TypeContext& context, int64_t value, IntType::numeric_precision presicion): 
      IntegerLiteral(0) {
      this->value = std::make_shared<IntValue>(value);
      getType() = QualType(IntType::get(&context, presicion, IntType::Signed));
    }
    int64_t& IntegerLiteral::getValue() {
      return value->template getAs<IntValue>().getValue();
    }
    int64_t IntegerLiteral::getValue() const {
      if (value)
        return value->template getAs<IntValue>().getValue();
      return 0;
    }
  }]>;
}

def UIntegerLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    UIntegerLiteral(TypeContext& context, const std::string& value, IntType::numeric_precision presicion);
    UIntegerLiteral(TypeContext& context, uint64_t value, IntType::numeric_precision presicion);
    uint64_t& getValue();
    uint64_t getValue() const;
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    UIntegerLiteral::UIntegerLiteral(TypeContext& context, const std::string& value, IntType::numeric_precision presicion): 
      UIntegerLiteral(0) {
      this->value = std::make_shared<UIntValue>(std::stoull(value));
      getType() = QualType(IntType::get(&context, presicion, IntType::Unsigned));
    }
    UIntegerLiteral::UIntegerLiteral(TypeContext& context, uint64_t value, IntType::numeric_precision presicion): 
      UIntegerLiteral(0) {
      this->value = std::make_shared<UIntValue>(value);
      getType() = QualType(IntType::get(&context, presicion, IntType::Unsigned));
    }
    uint64_t& UIntegerLiteral::getValue() {
      return value->template getAs<UIntValue>().getValue();
    }
    uint64_t UIntegerLiteral::getValue() const {
      if (value)
        return value->template getAs<UIntValue>().getValue();
      return 0;
    }
  }]>;
}

def FloatLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    FloatLiteral(TypeContext& context, const std::string& value, FloatType::numeric_precision presicion);
    FloatLiteral(TypeContext& context, double value, FloatType::numeric_precision presicion);
    double& getValue();
    double getValue() const;
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    FloatLiteral::FloatLiteral(TypeContext& context, const std::string& value, FloatType::numeric_precision presicion): 
      FloatLiteral(0) {
      this->value = std::make_shared<FloatValue>(std::stod(value));
      getType() = QualType(FloatType::get(&context, presicion));
    }
    FloatLiteral::FloatLiteral(TypeContext& context, double value, FloatType::numeric_precision presicion): 
      FloatLiteral(0) {
      this->value = std::make_shared<FloatValue>(value);
      getType() = QualType(FloatType::get(&context, presicion));
    }
    double& FloatLiteral::getValue() {
      return value->template getAs<FloatValue>().getValue();
    }
    double FloatLiteral::getValue() const {
      if (value)
        return value->template getAs<FloatValue>().getValue();
      return 0.;
    }
  }]>;
}

def ComplexLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def StringLiteral : AbstractNode<LiteralExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
  ClassSection header = ClassSection<Header, Public, [{
    StringLiteral(TypeContext& context, const std::string& value);
    std::string getValue() const;
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    StringLiteral::StringLiteral(TypeContext& context, const std::string& value): 
      StringLiteral(0) {
      this->value = std::make_shared<StringValue>(value);
      getType() = QualType(StringType::get(&context));
    }
    std::string StringLiteral::getValue() const {
      if (value)
        return value->template getAs<StringValue>().getValue();
      return "";
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
    EM<"CompoundPlus">,
    EM<"CompoundMinus">,
    EM<"CompoundMultiply">,
    EM<"CompoundDivide">,
    EM<"Modulo">,
    EM<"At">,
    EM<"Dot">,
    EM<"Comma">,
    EM<"Colon">,
    EM<"Semicolon">,
    EM<"Ternary">
  ]>;
  Variable op = EVar<[{Operator}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
}

def TernaryOperator : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child condition = Child<Dynamic, [{Expr}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
}

def TupleExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child elements = Child<DynamicList, [{Expr}]>;
}

def IdExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Level = Enum<[EM<"Scalar", [{1}]>, EM<"Vector", [{2}]>, EM<"Matrix", [{4}]>]>;
  Enum Coordinate = Enum<[EM<"X">, EM<"Y">, EM<"Z">]>;
  Variable context = Var<[{reference<ParallelConstruct>}]>;
  Variable level = EVar<[{Level}]>;
  Variable coordinate = EVar<[{Coordinate}]>;
  ClassSection header = ClassSection<Header, Public, [{
    IdExpr(const std::pair<int, int>& level, int coordinate);
    static Level getLevel(const std::pair<int, int>& level);
    static Coordinate getCoordinate(int coordinate);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    IdExpr::IdExpr(const std::pair<int, int>& level, int coordinate): IdExpr(nullptr, getLevel(level), getCoordinate(coordinate)) {}
    IdExpr::Level IdExpr::getLevel(const std::pair<int, int>& l) {
      if (l.first == -1)
        return static_cast<Level>(Scalar | Vector | Matrix);
      else if (l.first == 0 && l.second == -1)
        return static_cast<Level>(Scalar);
      else if (l.first == 1 && l.second == -1)
        return static_cast<Level>(Vector);
      else if (l.first == 2 && l.second == -1)
        return static_cast<Level>(Matrix);
      else if (l.first == 1 && l.second == 0)
        return static_cast<Level>(Vector | Scalar);
      else if (l.first == 2 && l.second == 0)
        return static_cast<Level>(Matrix | Scalar);
      else if (l.first == 2 && l.second == 1)
        return static_cast<Level>(Matrix | Vector);
      else
        throw(std::runtime_error("Invalid combination of levels"));
    }
    IdExpr::Coordinate IdExpr::getCoordinate(int coordinate) {
      if (coordinate == 1) return Y;
      else if (coordinate == 2) return Z;
      return X;
    }
  }]>;
}

def DimExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Level = Enum<[EM<"Scalar", [{1}]>, EM<"Vector", [{2}]>, EM<"Matrix", [{4}]>, EM<"Tensor", [{8}]>]>;
  Enum Coordinate = Enum<[EM<"X">, EM<"Y">, EM<"Z">]>;
  Variable context = Var<[{reference<ParallelConstruct>}]>;
  Variable level = EVar<[{Level}]>;
  Variable coordinate = EVar<[{Coordinate}]>;
  ClassSection header = ClassSection<Header, Public, [{
    DimExpr(const std::pair<int, int>& level, int coordinate);
    static Level getLevel(const std::pair<int, int>& level);
    static Coordinate getCoordinate(int coordinate);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    DimExpr::DimExpr(const std::pair<int, int>& level, int coordinate): DimExpr(nullptr, getLevel(level), getCoordinate(coordinate)) {}
    DimExpr::Level DimExpr::getLevel(const std::pair<int, int>& l) {
      if (l.first == -1)
        return static_cast<Level>(Scalar | Vector | Matrix | Tensor);
      else if (l.first == 0 && l.second == -1)
        return static_cast<Level>(Scalar);
      else if (l.first == 1 && l.second == -1)
        return static_cast<Level>(Vector);
      else if (l.first == 2 && l.second == -1)
        return static_cast<Level>(Matrix);
      else if (l.first == 3 && l.second == -1)
        return static_cast<Level>(Tensor);
      else if (l.first == 2 && l.second == 1)
        return static_cast<Level>(Matrix | Vector);
      else
        throw(std::runtime_error("Invalid combination of levels"));
    }
    DimExpr::Coordinate DimExpr::getCoordinate(int coordinate) {
      if (coordinate == 1) return Y;
      else if (coordinate == 2) return Z;
      return X;
    }
  }]>;
}

def DeclRefExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable identifier = Var<[{Identifier}]>;
  Variable decl = Var<[{reference<Decl>}]>;
  ClassSection header = ClassSection<Header, Public, [{
    DeclRefExpr(const Identifier &identifier) : DeclRefExpr(identifier, nullptr) {
    }
    bool isResolved() const {
      return decl;
    }
  }]>;
}

def MemberExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable member = Var<[{reference<Expr>}]>;
  Child owner = Child<Dynamic, [{Expr}]>;
}

def CallExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child callee = Child<Dynamic, [{Expr}]>;
  Child args = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    CallExpr(std::vector<Expr *> &&args) : CallExpr(nullptr, std::forward<std::vector<Expr *>>(args)) {
    }
  }]>;
}

def MemberCallExpr : AbstractNode<CallExpr, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ArrayExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child array = Child<Dynamic, [{Expr}]>;
  Child index = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ArrayExpr(std::vector<Expr *> &&index) : ArrayExpr(nullptr, std::forward<std::vector<Expr *>>(index)) {
    }
  }]>;
}

def CastExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    CastExpr(Expr* expr, QualType&& type);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    CastExpr::CastExpr(Expr* expr, QualType&& type): 
      CastExpr(expr) {
      getType() = type;
    }
  }]>;
}

def ImplicitCastExpr : AbstractNode<CastExpr, /* Abstract = */ 0, /* Implicit = */ 1> {
  ClassSection header = ClassSection<Header, Public, [{
    ImplicitCastExpr(Expr* expr, QualType&& type);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    ImplicitCastExpr::ImplicitCastExpr(Expr* expr, QualType&& type): 
      ImplicitCastExpr(CastExpr(expr, std::forward<QualType>(type))) {
    }
  }]>;
}

def RangeExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child start = Child<Dynamic, [{Expr}]>;
  Child step = Child<Dynamic, [{Expr}]>;
  Child stop = Child<Dynamic, [{Expr}]>;
}

def AllocateExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

def ReduceExpr : AbstractNode<Expr, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child expr = Child<Dynamic, [{Expr}]>;
}

#endif
