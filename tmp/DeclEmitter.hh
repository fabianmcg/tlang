#ifndef CODEGEN_DECLEMITTER_HH
#define CODEGEN_DECLEMITTER_HH

#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/CodeEmitterContext.hh>
#include <CodeGen/Emitter.hh>
#include <CodeGen/Traits.hh>
#include <CodeGen/StmtEmitter.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class DeclEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  IRType_t<Decl> emitDecl(Decl *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
};
} // namespace impl
class DeclEmitter: public CodeEmitterContext, public EmitterTable {
public:
  DeclEmitter(Emitter &emitter, TypeEmitter &type_emitter, StmtEmitterVisitor &stmt_emitter);
  llvm::AllocaInst* makeVariable(VariableDecl *variable, const std::string &suffix = "");
  IRType_t<FunctionType> makeFunctionType(FunctorDecl *functor);
  IRType_t<FunctorDecl> makeFunction(FunctorDecl *functor);
protected:
  TypeEmitter &typeEmitter;
  StmtEmitterVisitor &stmtEmitter;
};
class DeclEmitterVisitor: public DeclEmitter, public impl::DeclEmitterVisitor<DeclEmitterVisitor> {
public:
  using tlang::codegen::DeclEmitter::DeclEmitter;
  IRType_t<UnitDecl> emitUnitDecl(UnitDecl *unit);
  IRType_t<ModuleDecl> emitModuleDecl(ModuleDecl *module);
  IRType_t<FunctionDecl> emitFunctionDecl(FunctionDecl *function);
  IRType_t<VariableDecl> emitVariableDecl(VariableDecl *variable);
  IRType_t<ParameterDecl> emitParameterDecl(ParameterDecl *variable);
};
} // namespace tlang::codegen

#endif
