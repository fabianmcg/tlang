#ifndef IO_CXX_HH
#define IO_CXX_HH

#include "AST/Attr.hh"
#include "AST/Decl.hh"
#include "AST/Expr.hh"
#include "AST/Type.hh"
#include "AST/Stmt.hh"
#include <Support/Format.hh>
#include <Support/Enumerate.hh>

namespace tlang {
template <typename Derived>
class CXXIOVisitorBase {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
  /******************************************************************************
   * Emit types
   ******************************************************************************/
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit declarations
   ******************************************************************************/
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit statements
   ******************************************************************************/
#define NO_ABSTRACT
#define STMT(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Dynamic dispatchers
   ******************************************************************************/
  std::string emitType(Type *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
  std::string emitDecl(Decl *node) {
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
  std::string emitExpr(Expr *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
  std::string emitStmt(Stmt *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define STMT(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
};
struct CXXIOVisitor: public CXXIOVisitorBase<CXXIOVisitor> {
  std::string emitBoolType(BoolType *type) {
    return "bool";
  }
  std::string emitAddressType(AddressType *type) {
    return "int8_t*";
  }
  std::string emitIntType(IntType *type) {
    switch (type->getPrecision()) {
    case IntType::P_8:
      return type->isSigned() ? "int8_t" : "uint8_t";
    case IntType::P_16:
      return type->isSigned() ? "int16_t" : "uint16_t";
    case IntType::P_32:
      return type->isSigned() ? "int32_t" : "uint32_t";
    case IntType::P_64:
      return type->isSigned() ? "int64_t" : "uint64_t";
    default:
      return type->isSigned() ? "int" : "uint";
    }
  }
  std::string emitFloatType(FloatType *type) {
    switch (type->getPrecision()) {
    case FloatType::P_8:
      return "float";
    case FloatType::P_16:
      return "float";
    case FloatType::P_32:
      return "float";
    case FloatType::P_64:
      return "double";
    default:
      return "float";
    }
  }
  std::string emitPtrType(PtrType *type) {
    return frmt("{}*", emitType(type->getUnderlying()));
  }
  std::string emitQualType(QualType type) {
    auto base = type.getType() ? emitType(type.getType()) : "void";
    return type.isReference() ? frmt("{}*", base) : base;
  }
  std::string emitParameterDecl(ParameterDecl *decl) {
    return frmt("{} {}", emitQualType(decl->getType()), decl->getIdentifier());
  }
  std::string emitExternFunctionDecl(ExternFunctionDecl *function) {
    std::string args = "";
    auto sz = function->getParameters().size();
    for (auto [i, arg] : tlang::enumerate(function->getParameters())) {
      args += (i + 1 < sz) ? frmt("{}, ", emitParameterDecl(arg)) : emitParameterDecl(arg);
    }
    std::string fn = frmt("extern \"C\"\n{} {}({})", emitQualType(function->getReturnType()), function->getIdentifier(), args);
    return fn;
  }
};
}

#endif
