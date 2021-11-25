# Language properties

## Special functions

```
getContext(Identifier?)
getId()
```
## Parallel constructs

### Parallel operator
```
@<NumericExpr,*> CallExpr

CallExpr.1 (@ CallExpr.x)+
```

### Sync and Async operators
```
sync            // Syncs context
sync Context    // Syncs context Context
sync CallExpr
async CallExpr
```

### Sync and Async operators
```
sync CallExpr
async CallExpr
```

# Language constructs:

## Declarations:

### Task declaration
```
TaskDecl ::= task <(Attr Identifier)*> Identifier ([Type]? [Qualifiers]? Identifier,*) depend(...) weak_depend(...) provides(...) Stmt
```


### Function declaration
```
FunctionDecl ::= function <(Attr Identifier)*> Identifier ([Type]? [Qualifiers]? Identifier,*) Stmt
```

### Struct declaration
```
StructDecl ::= struct <(Attr Identifier)*> Identifier [: Struct.Id*]? {
  VarDecl;*
  MethodDecl*
  ConstructorDecl*
  DestructorDecl?
};
```

## Statements:

### If statement
```
IfStmt ::= if [constexpr]? ( Expr ) Stmt else Stmt
```

### While statement
```
WhileStmt ::= while ( Expr ) Stmt
```

### For statement
```
ForStmt ::= for ( Var in RangeExpr* ) Stmt
        |   for ( Var in Container ) Stmt
```

### Loop statement
```
LoopStmt ::= loop ( Var in RangeExpr* ) TaskStmt
         |   loop ( Var in Container ) TaskStmt
```

## Expressions:

### Binary expressions
```
BinaryOp ::= Expr (* or /) Expr
         |   Expr (+ or -) Expr
```