#ifndef AST_DECLCONTEXT_HH
#define AST_DECLCONTEXT_HH

#include <list>
#include <type_traits>
#include <ADT/SymbolTable/BasicTable.hh>
#include <ADT/SymbolTable/SymbolTable.hh>

namespace tlang {
class Decl;
class ModuleDecl;
class UnitContext: public symbol_table::interface::UniversalSymbolTable<std::string, symbol_table::interface::UniversalSymbol<Decl>> {
public:
  void add(ModuleDecl *decl);
protected:
  symbol_type search(const key_type &key) const;
private:
  std::list<symbol_type> modules;
};

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
struct IsDeclContext<UnitContext> : std::true_type {
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
