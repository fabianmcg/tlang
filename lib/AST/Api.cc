#include <AST/Api.hh>

namespace tlang {

ExternFunctionDecl* ASTApi::CreateExternFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters) {
  ExternFunctionDecl function = ExternFunctionDecl(FunctorDecl(identifier, std::move(returnType), std::move(parameters), nullptr));
  function.setType(context.types());
  return context.create<ExternFunctionDecl>(std::move(function));
}
FunctionDecl* ASTApi::CreateFunction(const Identifier &identifier, QualType returnType, List<ParameterDecl*> &&parameters) {
  FunctionDecl function = FunctionDecl(FunctorDecl(identifier, std::move(returnType), std::move(parameters), context.make<CompoundStmt>()));
  static_cast<UniversalContext*>(function.getBody())->getParent() = static_cast<UniversalContext*>(&function);
  function.setType(context.types());
  return context.create<FunctionDecl>(std::move(function));
}
ParameterDecl* ASTApi::CreateParameter(const Identifier &identifier, QualType type) {
  return context.make<ParameterDecl>(VariableDecl(identifier, std::move(type)));
}
}
