#ifndef CODEGEN_STMTEMITTER_HH
#define CODEGEN_STMTEMITTER_HH

#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/CodeEmitterContext.hh>
#include <CodeGen/Emitter.hh>
#include <CodeGen/ExprEmitter.hh>
#include <CodeGen/Traits.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class StmtEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
  IRType_t<CompoundStmt> emitCompoundStmt(CompoundStmt *node) {
    IRType_t<CompoundStmt> last { };
    auto &stmts = node->getStmts();
    for (auto &stmt : stmts)
      last = emitStmt(stmt);
    return last;
  }
#define VISIT_COMPOUNDSTMT 0
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)
#define STMT(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  IRType_t<Expr> emitExpr(Expr *node) {
    return nullptr;
  }
  IRType_t<Stmt> emitStmt(Stmt *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)
#define STMT(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      if (Expr::classof(node->classof()))
        return derived.emitExpr(static_cast<Expr*>(node));
      return nullptr;
    }
  }
};
} // namespace impl
class StmtEmitter: public CodeEmitterContext, public EmitterTable {
public:
  StmtEmitter(Emitter &emitter, TypeEmitter &type_emitter, ExprEmitterVisitor &expr_emitter);
  inline IRType_t<QualType> emitQualType(QualType &type) {
    return typeEmitter.emitQualType(type);
  }
  inline llvm::Value* emitBranch(llvm::BasicBlock *current_block, llvm::BasicBlock *to_block) {
    if (current_block && !current_block->getTerminator())
      return builder.CreateBr(to_block);
    return nullptr;
  }
  inline llvm::Value* emitBranch(llvm::BasicBlock *to_block) {
    return emitBranch(builder.GetInsertBlock(), to_block);
  }
  inline std::string makeLabel(ASTKind kind, const std::string label) {
    return label + "." + std::to_string(counters[kind]);
  }
  inline void incrementLabel(ASTKind kind) {
    counters[kind]++;
  }
  void resetCounters() {
    counters.clear();
  }
protected:
  TypeEmitter &typeEmitter;
  ExprEmitterVisitor &exprEmitter;
  std::map<ASTKind, int> counters;
};
class StmtEmitterVisitor: public StmtEmitter, public impl::StmtEmitterVisitor<StmtEmitterVisitor> {
public:
  using tlang::codegen::StmtEmitter::StmtEmitter;
  IRType_t<IfStmt> emitIfStmt(IfStmt *stmt);
  IRType_t<ForStmt> emitForStmt(ForStmt *stmt);
  IRType_t<WhileStmt> emitWhileStmt(WhileStmt *stmt);
  IRType_t<BreakStmt> emitBreakStmt(BreakStmt *stmt);
  IRType_t<ContinueStmt> emitContinueStmt(ContinueStmt *stmt);
  IRType_t<ReturnStmt> emitReturnStmt(ReturnStmt *stmt);
  IRType_t<SyncStmt> emitSyncStmt(SyncStmt *stmt);
  inline IRType_t<Expr> emitExpr(Expr *node) {
    return exprEmitter.emitExpr(node);
  }
private:
  llvm::BasicBlock *end_block { };
};
} // namespace tlang::codegen
#endif
