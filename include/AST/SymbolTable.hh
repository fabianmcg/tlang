#ifndef __AST_SYMBOLTABLE_HH__
#define __AST_SYMBOLTABLE_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"
#include "Decl.hh"
#include <map>

namespace _astnp_ {
struct SymbolTable {
  using id_type = std::string;
  using symbol_type = Decl*;
  std::map<id_type, symbol_type> table { };
  SymbolTable *parent { };
  SymbolTable() = default;
  ~SymbolTable() = default;
  SymbolTable(SymbolTable *p) :
      parent(p) {
  }
  void add(const id_type &id, symbol_type node) {
    table[id] = node;
  }
  void remove(const id_type &id) {
    auto it = table.find(id);
    if (it != table.end())
      table.erase(id);
  }
  symbol_type find(const id_type &id, bool local = false) const {
    auto it = table.find(id);
    if (it != table.end())
      return it->second;
    if (!local && parent)
      return parent->find(id, local);
    return nullptr;
  }
  void print(std::ostream &ost) const {
    ost << this << ":" << parent << ":" << std::endl;
    for (auto& [k, v] : table)
      ost << to_string(v->classOf()) << ": " << k << " " << v << std::endl;
  }
};
}
#endif
