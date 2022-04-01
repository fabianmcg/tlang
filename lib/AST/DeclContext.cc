#include "AST/DeclContext.hh"
#include "AST/Decl.hh"

void tlang::DeclContext::add(Decl *decl) {
  if (auto nd = dynamic_cast<NamedDecl*>(decl))
    parent_type::add(nd->getIdentifier(), std::forward<Decl*>(decl));
  else
    parent_type::add("", std::forward<Decl*>(decl));
}

void tlang::NoVisitDeclContext::add(Decl *decl) {
  if (auto nd = dynamic_cast<NamedDecl*>(decl))
    parent_type::add(nd->getIdentifier(), std::forward<Decl*>(decl));
  else
    parent_type::add("", std::forward<Decl*>(decl));
}

void tlang::VariableContext::add(Decl *decl) {
  if (auto nd = dynamic_cast<NamedDecl*>(decl))
    parent_type::add(nd->getIdentifier(), std::forward<Decl*>(decl));
  else
    parent_type::add("", std::forward<Decl*>(decl));
}
