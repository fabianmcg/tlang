#ifndef AST_API_HH
#define AST_API_HH

#include <AST/ASTContext.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <initializer_list>

namespace tlang {
struct ASTApi: ASTContextReference {
  using ASTContextReference::ASTContextReference;
  template <typename T, typename U>
  U* AddToContext(T *context, U *child) {
    context->add(child);
    static_cast<UniversalContext*>(child)->getParent() = static_cast<UniversalContext*>(context);
    return child;
  }
  QualType CreateVoid() {
    return QualType(nullptr);
  }
  template <typename T, typename ...Args>
  QualType CreateType(Args &&...args) {
    return context.makeType<T>(std::forward<Args>(args)...);
  }
  template <typename T, typename ...Args>
  QualType CreateQualType(QualType::cvr_qualifiers quals, Args &&...args) {
    return context.makeQualType<T>(quals, 0, std::forward<Args>(args)...);
  }
  template <typename T, typename ...Args>
  QualType CreateFQType(QualType::cvr_qualifiers quals, int as, Args &&...args) {
    QualType type = context.makeQualType<T>(quals, as, std::forward<Args>(args)...);
    return type;
  }
  QualType CreateAddressType(int as = 0) {
    return CreateFQType<AddressType>(QualType::None, as);
  }
  VariableDecl* CreateVariable(const Identifier &identifier, QualType type, Expr *init = nullptr, VariableDecl::StorageKind storage =
      VariableDecl::Generic);
  ParameterDecl* CreateParameter(const Identifier &identifier, QualType type);
  template <typename ...Args>
  List<ParameterDecl*> CreateParameterList(Args ...args) {
    return List<ParameterDecl*> { args... };
  }
  ExternFunctionDecl* CreateExternFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters);
  template <typename ...Args>
  ExternFunctionDecl* CreateExternFunction(const Identifier &identifier, QualType returnType, Args ...args) {
    return CreateExternFunction(identifier, returnType, CreateParameterList(std::forward<Args>(args)...));
  }
  FunctionDecl* CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters);
  FunctionDecl* CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters, CompoundStmt *stmt);
  template <typename ...Args>
  FunctionDecl* CreateFunction(const Identifier &identifier, QualType returnType, Args ...args) {
    return CreateFunction(identifier, returnType, CreateParameterList(std::forward<Args>(args)...));
  }
  CompoundStmt* CreateCompoundStmt(List<Stmt*> &&stmts);
  template <typename ...Args>
  CompoundStmt* CreateCompoundStmt(Args *...args) {
    return CreateCompoundStmt(List<Stmt*> { std::forward<Args>(args)... });
  }
  BooleanLiteral* CreateLiteral(bool value);
  IntegerLiteral* CreateLiteral(int64_t value, IntType::numeric_precision precision = IntType::Default);
  UIntegerLiteral* CreateLiteral(uint64_t value, IntType::numeric_precision precision = IntType::Default);
  FloatLiteral* CreateLiteral(double value, FloatType::numeric_precision precision = FloatType::Default);
  DeclRefExpr* CreateDeclRefExpr(ValueDecl *decl);
  CallExpr* CreateCallExpr(DeclRefExpr *callee, List<Expr*> &&args);
  template <typename ...Args>
  CallExpr* CreateCallExpr(DeclRefExpr *callee, Args *...args) {
    return CreateCallExpr(callee, List<Expr*> { args... });
  }
  ParenExpr* CreateParenExpr(Expr *expr);
  BinaryOperator* CreateBinOp(BinaryOperator::Operator op, Expr *lhs, Expr *rhs);
  UnaryOperator* CreateUnOp(UnaryOperator::Operator op, Expr *expr);
  DeclStmt* CreateDeclStmt(List<VariableDecl*> &&variables);
  template <typename ...VD>
  DeclStmt* CreateDeclStmt(VariableDecl *var, VD *...vars) {
    return CreateDeclStmt(List<VariableDecl*> { var, vars... });
  }
  CastExpr* CreateCast(Expr* expr, QualType type);
  ImplicitCastExpr* CreateImplicitCast(Expr* expr, QualType type);
  CXXDecl* CreateCXX(const std::string& code);
  Stmt* PrependStmt(CompoundStmt *cs, Stmt *stmt);
  Stmt* AppendStmt(CompoundStmt *cs, Stmt *stmt);
  template <typename T, typename ...Args>
  T* CreateNode(Args &&...args) {
    return context.make<T>(std::forward<Args>(args)...);
  }
};
}

#endif
