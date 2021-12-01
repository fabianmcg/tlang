# Language properties

There are 3 main classes of constructs:
- Contexts
- Policies
- Tasks

## Contexts

Contexts are one of the most important features of `tlang` as they are in charge of:
- Providing information on how to initiate and manage parallelism.
  - E.g: how to initiate host threads, open a CUDA kernel, etc.
- Providing information w.r.t. memory operations and memory address spaces.
  - E.g: how to allocate device memory, if the address space can be accessed by other contexts, etc.
- Get parallelism information.
  - E.g: number of threads being used, the current thread id, etc.
- Specify which parallel intrinsics are available within the context:
  - E.g: atomics, reduces, barriers, mutexes and others.

All `Policies`, `Tasks` & `Functions`  require a valid context in order to be compiled and executed. Furthermore contexts are propagated in a top down manner, i.e. functions called from a certain context automatically inherit the context.

Examples of possible contexts are:
- Serial context.
- MPI context.
- PThreads context.
- ArgoBots context.
- HIP context.
- CUDA context.
- CUDAGraph context.

Other features of contexts are:
- They can have an internal state, thus the following contexts are possible within the language:
  - A context that allocates memory using a pool allocator. 
  - Replayable contexts -can be constructed once and called multiple times, like CUDAgraphs. 

## Policies

Policies interact with contexts and handle the actual parallel patterns executed by the program. The most basic policy is the `loop` policy:


### Loop policy
```
loop (int x in 0:n, int y in x:n)
  A[x, y] += A[x, y] + alpha * B[x, y]; 
```
`loop`'s written in the above manner tell the compiler to collapse the iterations and launch them in parallel.  


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