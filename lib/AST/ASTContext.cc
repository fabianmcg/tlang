#include <AST/ASTContext.hh>

namespace tlang {
ASTContext::ASTContext() {
  program = make<UniverseDecl>();
}
UniverseDecl* ASTContext::operator*() const {
  return program;
}
void ASTContext::remove(ASTNode *node) {
  auto it = nodes.find(reinterpret_cast<std::uintptr_t>(node));
  if (it != nodes.end())
    nodes.erase(it);
}
void ASTContext::addModule(UnitDecl *unit, ModuleDecl *module) {
  unit->add(module);
}
TypeContext& ASTContext::types() {
  return type_context;
}
}
