#ifndef AST_DECLCONTEXT_HH
#define AST_DECLCONTEXT_HH

#include <list>
#include <type_traits>
#include <ADT/SymbolTable/SymbolTable.hh>
#include <ADT/SymbolTable/UniqueTable.hh>

namespace tlang {
class Decl;
class ModuleDecl;
using UniversalContext = symbol_table::UniversalSymbolTable<std::string, Decl>;
class UnitContext: public symbol_table::UniversalSymbolTable<std::string, Decl> {
public:
  void add(ModuleDecl *decl);
protected:
  symbol_type search(const key_type &key) const;
private:
  std::list<symbol_type> modules;
};

class DeclContext: public symbol_table::ASTSymbolTable<std::string, Decl> {
public:
  using parent_type = symbol_table::ASTSymbolTable<std::string, Decl>;
  using parent_type::ASTSymbolTable;
  void add(Decl *decl);
};
class NoVisitDeclContext: public symbol_table::ASTSymbolTable<std::string, Decl> {
public:
  using parent_type = symbol_table::ASTSymbolTable<std::string, Decl>;
  NoVisitDeclContext() {
    visitContext = false;
  }
  void add(Decl *decl);
};
class VariableContext: public symbol_table::UniqueTable<std::string, Decl> {
public:
  using parent_type = symbol_table::UniqueTable<std::string, Decl>;
  using parent_type::UniqueTable;
  void add(Decl *decl);
};

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
