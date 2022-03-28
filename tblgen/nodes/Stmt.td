#ifndef STMT_TD
#define STMT_TD
include "ASTNode.td"

def Stmt : AbstractNode<ASTNode, /* Abstract = */ 1, /* Implicit = */ 0, /* Top = */ 1> {
}

def CompoundStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmts = Child<DynamicList, [{Stmt}]>;
}

def ValueStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def DeclStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child decl = Child<DynamicList, [{VariableDecl}]>;
}

def AttributedStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child attributes = Child<Dynamic, [{AttrList}]>;
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def IfStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable Constexpr = Var<[{bool}]>;
  Child condition = Child<Dynamic, [{Expr}]>;
  Child thenStmt = Child<Dynamic, [{Stmt}]>;
  Child elseStmt = Child<Dynamic, [{Stmt}]>;
}

def ForStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child range = Child<DynamicList, [{RangeStmt}]>;
  Child body = Child<Dynamic, [{Stmt}]>;
}

def WhileStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child condition = Child<Dynamic, [{Expr}]>;
  Child body = Child<Dynamic, [{Stmt}]>;
}

def RangeStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child decl = Child<Dynamic, [{VariableDecl}]>;
  Child variable = Child<Dynamic, [{DeclRefExpr}]>;
  Child range = Child<Dynamic, [{RangeExpr}]>;
}

def BreakStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ContinueStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def ReturnStmt : AbstractNode<ValueStmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child return = Child<Dynamic, [{Expr}]>;
}

def ParallelStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable referencedPrivate = Var<[{VariableList}]>;
  Variable referencedShared = Var<[{VariableList}]>;
  Child privateVariables = Child<DynamicList, [{Expr}]>;
  Child sharedVariables = Child<DynamicList, [{Expr}]>;
  Child stmt = Child<Dynamic, [{Stmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    using VariableList = std::set<VariableDecl *>;
    template <typename... Args> ParallelStmt(Args &&...args) : __children(std::forward<Args>(args)...) {
    }
  }]>;
}

def LoopStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable noWait = Var<[{bool}]>;
  Child reduceList = Child<Dynamic, [{ReduceStmt}]>;
  Child ranges = Child<DynamicList, [{RangeStmt}]>;
  Child body = Child<Dynamic, [{Stmt}]>;
}

def SyncStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
}

def SingleStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def MainStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def AtomicStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable operator = Var<[{OperatorKind}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    AtomicStmt(OperatorKind op, Expr *lhs, Expr *rhs) : Stmt(), __operator(op), __children(lhs, rhs) {
    }
  }]>;
}

def ReduceStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable operator = Var<[{OperatorKind}]>;
  Child exprs = Child<DynamicList, [{Expr}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ReduceStmt(OperatorKind op, std::vector<Expr *> &&exprs)
        : Stmt(), __operator(op), __children(std::forward<std::vector<Expr *>>(exprs)) {
    }
  }]>;
}

#endif
