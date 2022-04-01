#ifndef AST_DECLCONTEXT_HH
#define AST_DECLCONTEXT_HH

#include <ADT/SymbolTable/SymbolTable.hh>
#include <ADT/SymbolTable/BasicTable.hh>
#include <type_traits>

namespace tlang {
class Decl;
class DeclContext: public symbol_table::ASTSymbolTable<Decl> {
public:
  using parent_type = symbol_table::ASTSymbolTable<Decl>;
  using parent_type::ASTSymbolTable;
  void add(Decl *decl);
};
class NoVisitDeclContext: public symbol_table::ASTSymbolTable<Decl> {
public:
  using parent_type = symbol_table::ASTSymbolTable<Decl>;
  NoVisitDeclContext() {
    visitContext = false;
  }
  void add(Decl *decl);
};
class VariableContext: public symbol_table::BasicTable<Decl> {
public:
  using parent_type = symbol_table::BasicTable<Decl>;
  using parent_type::BasicTable;
  void add(Decl *decl);
};

using UniversalSymbolTable = typename DeclContext::universal_symbol_table;

template <typename T>
struct IsDeclContext: std::false_type {
};

template <>
struct IsDeclContext<DeclContext> : std::true_type {
};

template <>
struct IsDeclContext<NoVisitDeclContext> : std::true_type {
};

template <>
struct IsDeclContext<VariableContext> : std::true_type {
};

}
#endif
