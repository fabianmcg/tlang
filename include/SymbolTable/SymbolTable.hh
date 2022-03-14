#ifndef __SYMBOL_TABLE_HH__
#define __SYMBOL_TABLE_HH__

#include "TableInterface.hh"
#include "IteratorsInterface.hh"
#include <iostream>
#include <map>
#include <string>

namespace tlang::symbol_table {
template <typename ASTNode>
struct ASTSymbol {
  using OrderedSymbolIterator = interface::OrderedSymbolIteratorInterface<ASTSymbol>;
  using ReverseOrderedSymbolIterator = interface::ReverseOrderedSymbolIteratorInterface<ASTSymbol>;
  using OverloadSymbolIterator = interface::OverloadSymbolIteratorInterface<ASTSymbol>;
  ASTSymbol() = default;
  inline ASTSymbol(ASTNode *node) :
      node(node) {
  }
  inline operator bool() const {
    return node;
  }
  inline ASTNode* operator*() const {
    return node;
  }
  template <typename T>
  inline T* getAs() const {
    return static_cast<T*>(node);
  }
  template <typename T>
  inline T* getAsDyn() const {
    return dynamic_cast<T*>(node);
  }
  inline OrderedSymbolIterator getOrderedIterator() const {
    return OrderedSymbolIterator { const_cast<ASTSymbol*>(this) };
  }
  inline ReverseOrderedSymbolIterator getReverseOrderedIterator() const {
    return ReverseOrderedSymbolIterator { const_cast<ASTSymbol*>(this) };
  }
  inline OverloadSymbolIterator getOverloadIterator() const {
    return OverloadSymbolIterator { const_cast<ASTSymbol*>(this) };
  }
  inline void print(std::ostream &ost) const {
    ost << node << ": " << prev << " -> " << next << " |-> " << overload;
  }
  inline friend std::ostream& operator<<(std::ostream &ost, const ASTSymbol &symbol) {
    symbol.print(ost);
    return ost;
  }
  ASTNode *node { };
  ASTSymbol *prev { };
  ASTSymbol *next { };
  ASTSymbol *overload { };
  int depth { };
};
namespace interface {
template <typename Symbol, typename ASTNode>
class ASTSymbolTable: public interface::SymbolTable<ASTSymbolTable<Symbol, ASTNode>, std::string, ASTSymbol<ASTNode>, Symbol> {
public:
  using parent_type = interface::SymbolTable<ASTSymbolTable<Symbol, ASTNode>, std::string, ASTSymbol<ASTNode>, Symbol>;
  using key_type = typename parent_type::key_type;
  using universal_symbol_type = typename parent_type::universal_symbol_type;
  using value_type = ASTNode*;
  using symbol_type = Symbol;
  using table_type = std::multimap<key_type, symbol_type>;
  ASTSymbolTable() = default;
  ~ASTSymbolTable() = default;
  ASTSymbolTable(ASTSymbolTable&&) = default;
  ASTSymbolTable(const ASTSymbolTable&) = delete;
  ASTSymbolTable& operator=(ASTSymbolTable&&) = default;
  ASTSymbolTable& operator=(const ASTSymbolTable&) = delete;
  inline size_t size() const {
    return symbols.size();
  }
  bool add(const key_type &key, value_type &&value) {
    auto it = symbols.insert( { key, symbol_type { value } });
    auto &symbol = it->second;
    if (!this->first_symbol)
      this->first_symbol = &symbol;
    auto tmpLast = this->last_symbol;
    this->last_symbol = &symbol;
    if (tmpLast) {
      symbol.prev = tmpLast;
      tmpLast->next = this->last_symbol;
    }
    if (it != symbols.begin())
      if ((--it)->first == key)
        it->second.overload = &symbol;
    return true;
  }
  void remove(const key_type &key) {
    symbols.erase(key);
  }
  inline symbol_type find(const key_type &key) const {
    auto it = symbols.find(key);
    if (it != symbols.end())
      return it->second;
    return symbol_type { };
  }
  void print(std::ostream &ost) const {
    ost << this << "[" << this->parent << "]:" << std::endl;
    for (auto& [k, v] : symbols)
      ost << "\t" << k << ": [" << v << "]" << std::endl;
  }
protected:
  virtual universal_symbol_type search(const key_type &key) const {
    return find(key);
  }
  table_type symbols { };
};
}
template <typename ASTNode>
using ASTSymbolTable = interface::ASTSymbolTable<ASTSymbol<ASTNode>, ASTNode>;
}
#endif
