#ifndef __AST_SYMBOLTABLE_HH__
#define __AST_SYMBOLTABLE_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"
#include "Decl.hh"
#include "ASTNode.hh"
#include "TypeContext.hh"
#include <map>

namespace _astnp_ {

struct SymbolTable {
  std::map<std::string, Decl*> context { };
  SymbolTable *parent { };
  SymbolTable() = default;
  ~SymbolTable() = default;
  SymbolTable(SymbolTable *p) :
      parent(p) {
  }
  void add(const std::string&, ASTNode *node) {

  }
  void remove(const std::string&) {

  }
  Decl* find(const std::string&, bool local) {
    return nullptr;
  }
};
}
#endif
