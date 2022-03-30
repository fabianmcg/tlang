#ifndef __CLASS_TABLE_HH__
#define __CLASS_TABLE_HH__

#include "SymbolTable.hh"

namespace tlang::symbol_table {
template <typename ASTSymbol>
class ClassSymbolIteratorInterface: public interface::ForwardSymbolIterator<ClassSymbolIteratorInterface<ASTSymbol>, ASTSymbol> {
  using parent_type = interface::ForwardSymbolIterator<ClassSymbolIteratorInterface<ASTSymbol>, ASTSymbol>;
  using symbol_type = ASTSymbol;
  using parent_type::ForwardSymbolIterator;
protected:
  inline void next() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->next_symbol;
  }
};
template <typename ASTNode>
struct ClassSymbol: ASTSymbol<ASTNode> {
  using OrderedSymbolIterator = interface::OrderedSymbolIteratorInterface<ClassSymbol>;
  using ReverseOrderedSymbolIterator = interface::ReverseOrderedSymbolIteratorInterface<ClassSymbol>;
  using OverloadSymbolIterator = interface::OverloadSymbolIteratorInterface<ClassSymbol>;
  using ASTSymbol<ASTNode>::ASTSymbol;
  inline OrderedSymbolIterator getOrderedIterator() const {
    return OrderedSymbolIterator { const_cast<ClassSymbol*>(this) };
  }
  inline ReverseOrderedSymbolIterator getReverseOrderedIterator() const {
    return ReverseOrderedSymbolIterator { const_cast<ClassSymbol*>(this) };
  }
  inline OverloadSymbolIterator getOverloadIterator() const {
    return OverloadSymbolIterator { const_cast<ClassSymbol*>(this) };
  }
  inline void print(std::ostream &ost) const {
    ost << this->node << ": " << this->prev << " -> " << this->next << " |-> " << this->overload;
  }
  inline friend std::ostream& operator<<(std::ostream &ost, const ClassSymbol &symbol) {
    symbol.print(ost);
    return ost;
  }
  ClassSymbol *next_symbol { };
};
template <typename ASTNode, typename MemberDecl, typename MethodDecl>
class ClassSymbolTable: public interface::ASTSymbolTable<ClassSymbol<ASTNode>, ASTNode> {
public:
  using parent_type = interface::ASTSymbolTable<ClassSymbol<ASTNode>, ASTNode>;
  using key_type = typename parent_type::key_type;
  using value_type = typename parent_type::value_type;
  using symbol_type = typename parent_type::symbol_type;
  ClassSymbolTable() = default;
  ~ClassSymbolTable() = default;
  ClassSymbolTable(ClassSymbolTable&&) = default;
  ClassSymbolTable(const ClassSymbolTable&) = delete;
  ClassSymbolTable& operator=(ClassSymbolTable&&) = default;
  ClassSymbolTable& operator=(const ClassSymbolTable&) = delete;
  bool add(const key_type &key, value_type &&value) {
    auto it = this->symbols.insert( { key, symbol_type { value } });
    auto &symbol = it->second;
    if (!this->first_symbol)
      this->first_symbol = &symbol;
    auto tmpLast = this->last_symbol;
    this->last_symbol = &symbol;
    if (tmpLast) {
      symbol->prev = tmpLast;
      tmpLast->next = this->last_symbol;
    }
    if (it != this->symbols.begin())
      if ((--it)->first == key)
        it->second.overload = &symbol;
    return true;
  }

protected:
  symbol_type *first_member { };
  symbol_type *last_member { };
  symbol_type *first_method { };
  symbol_type *last_method { };
};
}
#endif
