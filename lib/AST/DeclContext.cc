#include "AST/DeclContext.hh"
#include "AST/Decl.hh"
#include <AST/Traits.hh>

void tlang::UnitContext::add(ModuleDecl *decl) {
  if (decl) {
    modules.push_back(symbol_type { decl });
    auto &symbol = modules.back();
    if (!first_symbol)
      first_symbol = &symbol;
    auto tmpLast = last_symbol;
    last_symbol = &symbol;
    if (tmpLast) {
      symbol.prev = tmpLast;
      tmpLast->next = last_symbol;
    }
  }
}

tlang::UnitContext::symbol_type tlang::UnitContext::search(const key_type &key) const {
  for (auto table : modules)
    if (auto module = dyn_cast<ModuleDecl>(*table))
      if (auto symbol = module->find(key, true))
        return symbol;
  return symbol_type { };
}

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
