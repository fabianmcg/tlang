#include "AST/DeclContext.hh"
#include "AST/Decl.hh"
#include <AST/Traits.hh>

void tlang::ProgramContext::add(UnitDecl *decl) {
  if (decl) {
    auto &symbol = units[decl->getGenKind()] = symbol_type { decl };
    push_back(&symbol);
  }
}

tlang::UnitDecl* tlang::ProgramContext::get(int kind) {
  auto it = units.find(kind);
  if (it != units.end())
    return it->second.get<UnitDecl>();
  return nullptr;
}

tlang::ProgramContext::symbol_type tlang::ProgramContext::search(const key_type &key) const {
  return symbol_type { };
}

void tlang::UnitContext::add(ModuleDecl *decl) {
  if (decl) {
    modules.push_back(symbol_type { decl });
    auto &symbol = modules.back();
    push_back(&symbol);
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
