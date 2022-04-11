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
  U* AddToContext(T *context, U *otherContext) {
    context->add(otherContext);
    static_cast<UniversalContext*>(otherContext)->getParent() = static_cast<UniversalContext*>(context);
    return otherContext;
  }
  template <typename T, typename ...Args>
  QualType CreateType(Args &&...args) {
    return context.makeType<T>(std::forward<Args>(args)...);
  }
  template <typename T, typename ...Args>
  QualType CreateQualType(QualType::cvr_qualifiers quals, Args &&...args) {
    return context.makeQualType<T>(quals, std::forward<Args>(args)...);
  }
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
  FunctionDecl* CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters, CompoundStmt* stmt);
  template <typename ...Args>
  FunctionDecl* CreateFunction(const Identifier &identifier, QualType returnType, Args ...args) {
    return CreateFunction(identifier, returnType, CreateParameterList(std::forward<Args>(args)...));
  }
  ParenExpr* CreateParenExpr(Expr *expr);
  BinaryOperator* CreateBinOp(BinaryOperator::Operator op, Expr *lhs, Expr *rhs);
};
}

#endif
