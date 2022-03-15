#ifndef __DECL_CONTEXT_HH__
#define __DECL_CONTEXT_HH__

#include "SymbolTable/SymbolTable.hh"

namespace tlang {
class Decl;
class DeclContext: public symbol_table::ASTSymbolTable<Decl> {
public:
  using parent_type = symbol_table::ASTSymbolTable<Decl>;
  using parent_type::ASTSymbolTable;
  void add(Decl *decl);
};
using UniversalSymbolTable = typename DeclContext::universal_symbol_table;
}
#endif
