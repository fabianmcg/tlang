#ifndef STMT_TD
#define STMT_TD
include "ASTNode.td"
include "Decl.td"

def StmtHeader : CodeSection<Header, [{
#include <AST/DeclContext.hh>
}]>;

def StmtSource : CodeSection<Source, [{
#include <AST/Decl.hh>
}]>;

def Stmt : AbstractNode<ASTNode, /* Abstract = */ 1, /* Implicit = */ 0, /* Top = */ 1> {
}

def CompoundStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0>, NoVisitDeclContext {
  Child stmts = Child<DynamicList, [{Stmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    CompoundStmt(List<Stmt*>&& stmts);
    bool addStmt(Stmt* stmt);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{
    CompoundStmt::CompoundStmt(List<Stmt*>&& stmts): CompoundStmt(NoVisitDeclContext(), std::move(stmts)) {
    }
    bool CompoundStmt::addStmt(Stmt* stmt) {
      if (!stmt)
        return false;
      if (auto ds = dyn_cast<DeclStmt>(stmt)) {
        for (auto decl : ds->getDecl())
          add(decl);
      }
      getStmts().push_back(stmt);
      return true;
    }
  }]>;
}

def ValueStmt : AbstractNode<Stmt, /* Abstract = */ 1, /* Implicit = */ 0> {
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

def ForStmt : AbstractNode<Stmt, /* Abstract = */ 0, /* Implicit = */ 0>, VariableContext {
  Child ranges = Child<DynamicList, [{RangeStmt}]>;
  Child body = Child<Dynamic, [{Stmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    ForStmt(List<RangeStmt*>&& ranges, Stmt* stmt);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{  
    ForStmt::ForStmt(List<RangeStmt*>&& ranges, Stmt* stmt): ForStmt(VariableContext(), std::move(ranges), stmt) {
      auto &Ranges = getRanges();
      for (auto& range : Ranges)
        if (range->getDecl())
          add(range->getDecl());
    }
  }]>;
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


def ParallelBase : AbstractNode<Stmt, /* Abstract = */ 1, /* Implicit = */ 0> {
}

def ContextStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Kind = Enum<[EM<"Inherited">, EM<"Default">, EM<"CPU">, EM<"GPU">]>;
  Variable parentContext = Var<[{reference<ContextStmt>}]>;
  Variable contextKind = EVar<[{Kind}]>;
  Child mappedExprs = Child<DynamicList, [{MapStmt}]>;
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def MapStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Variable mapKind = EVar<[{Kind}]>;
  Child mappedExprs = Child<DynamicList, [{Expr}]>;
  Enum Kind = Enum<[EM<"to">, EM<"from">, EM<"toFrom">, EM<"create">]>;
}

def ParallelContext : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Mapping = Enum<[EM<"None">, EM<"Vector", [{1}]>, EM<"Matrix", [{2}]>, EM<"Tensor", [{4}]>, EM<"All", [{7}]>]>;
  Variable context = Var<[{reference<ContextStmt>}]>;
  Variable parentContext = Var<[{reference<ParallelContext>}]>;
  Variable mapping = EVar<[{Mapping}]>;
  Variable noWait = Var<[{bool}]>;
  Child firstPrivateVariables = Child<DynamicList, [{Expr}]>;
  Child privateVariables = Child<DynamicList, [{Expr}]>;
  Child sharedVariables = Child<DynamicList, [{Expr}]>;
  Child reduceList = Child<DynamicList, [{ReduceStmt}]>;
  Child mappedExprs = Child<DynamicList, [{MapStmt}]>;
  Child launchParameters = Child<DynamicList, [{TupleExpr}]>;
}

def ParallelStmt : AbstractNode<ParallelContext, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def LoopStmt : AbstractNode<ParallelContext, /* Abstract = */ 0, /* Implicit = */ 0>, VariableContext {
  Child ranges = Child<DynamicList, [{RangeStmt}]>;
  Child body = Child<Dynamic, [{Stmt}]>;
  ClassSection header = ClassSection<Header, Public, [{
    LoopStmt(ParallelContext&& context, List<RangeStmt*>&& ranges, Stmt* stmt);
  }]>;
  ClassSection source = ClassSection<Source, Public, [{  
    LoopStmt::LoopStmt(ParallelContext&& context, List<RangeStmt*>&& ranges, Stmt* stmt): LoopStmt(std::move(context), VariableContext(), std::move(ranges), stmt) {
      auto &Ranges = getRanges();
      for (auto& range : Ranges)
        if (range->getDecl())
          add(range->getDecl());
    }
  }]>;
}

def SyncStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Kind = Enum<[EM<"Default">, EM<"Vector", [{1}]>, EM<"Matrix", [{2}]>, EM<"Tensor", [{4}]>]>;
  Variable context = Var<[{reference<ParallelContext>}]>;
  Variable syncKind = EVar<[{Kind}]>;
}

def SingleStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def MainStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Child stmt = Child<Dynamic, [{Stmt}]>;
}

def AtomicStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Operator = Enum<[
    EM<"Plus">,
    EM<"Minus">,
    EM<"Multiply">,
    EM<"Divide">,
  ]>;
  Variable op = EVar<[{Operator}]>;
  Child lhs = Child<Dynamic, [{Expr}]>;
  Child rhs = Child<Dynamic, [{Expr}]>;
}

def ReduceStmt : AbstractNode<ParallelBase, /* Abstract = */ 0, /* Implicit = */ 0> {
  Enum Operator = Enum<[
    EM<"Plus">,
    EM<"Minus">,
    EM<"Multiply">,
    EM<"Divide">,
  ]>;
  Variable op = EVar<[{Operator}]>;
  Child exprs = Child<DynamicList, [{Expr}]>;
}

#endif
