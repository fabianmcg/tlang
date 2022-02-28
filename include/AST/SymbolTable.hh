#ifndef __AST_SYMBOLTABLE_HH__
#define __AST_SYMBOLTABLE_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"
#include "Decl.hh"
#include "ASTNode.hh"
#include "TypeContext.hh"
#include <list>
#include <map>

namespace _astnp_ {

struct SymbolTable {
  std::map<std::string, std::list<Decl*>> context { };
  SymbolTable *parent { };
  SymbolTable() = default;
  ~SymbolTable() = default;
  SymbolTable(SymbolTable *p) :
      parent(p) {
  }
  void add(const std::string& id, Decl *node) {
    if (node)
      context[id].push_back(node);
  }
  void remove(const std::string& id) {
    auto it = context.find(id);
    if (it != context.end())
      context.erase(id);
  }
  Decl* find(const std::string&, bool local) {
    return nullptr;
  }
};
}
#endif
