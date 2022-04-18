#ifndef ADT_SYMBOLTABLE_UNIVERSALTABLE_HH
#define ADT_SYMBOLTABLE_UNIVERSALTABLE_HH

#include <ADT/SymbolTable/Iterators.hh>
#include <ADT/SymbolTable/UniversalSymbol.hh>

namespace tlang::symbol_table {
template <typename Key, typename NodeType>
class UniversalSymbolTable {
public:
  using key_type = Key;
  using symbol_type = UniversalSymbol<NodeType>;
  using iterator = SymbolIterator<symbol_type>;
  using const_iterator = SymbolIterator<symbol_type, true>;
  using reverse_iterator = SymbolIterator<symbol_type, false, true>;
  using const_reverse_iterator = SymbolIterator<symbol_type, true, true>;
  UniversalSymbolTable() = default;
  virtual ~UniversalSymbolTable() = default;
  UniversalSymbolTable(UniversalSymbolTable&&) = default;
  UniversalSymbolTable(const UniversalSymbolTable&) = delete;
  UniversalSymbolTable& operator=(UniversalSymbolTable&&) = default;
  UniversalSymbolTable& operator=(const UniversalSymbolTable&) = delete;
  inline UniversalSymbolTable*& getParent() {
    return parent;
  }
  inline UniversalSymbolTable* getParent() const {
    return parent;
  }
  inline iterator begin() {
    if (first_symbol)
      return iterator { *first_symbol };
    return iterator { };
  }
  inline iterator end() {
    if (last_symbol)
      return iterator { last_symbol->makeEnd() };
    return iterator { };
  }
  inline const_iterator begin() const {
    if (first_symbol)
      return const_iterator { *first_symbol };
    return const_iterator { };
  }
  inline const_iterator end() const {
    if (last_symbol)
      return const_iterator { last_symbol->makeEnd() };
    return const_iterator { };
  }
  inline reverse_iterator rbegin() {
    if (last_symbol)
      return reverse_iterator { *last_symbol };
    return reverse_iterator { };
  }
  inline reverse_iterator rend() {
    if (first_symbol)
      return reverse_iterator { first_symbol->makeREnd() };
    return reverse_iterator { };
  }
  inline const_reverse_iterator rbegin() const {
    if (last_symbol)
      return const_reverse_iterator { *last_symbol };
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rend() const {
    if (first_symbol)
      return const_reverse_iterator { first_symbol->makeREnd() };
    return const_reverse_iterator { };
  }
  inline symbol_type find(const key_type &key, bool local) const {
    symbol_type symbol = search(key);
    if (symbol)
      return symbol;
    if (!local && parent)
      return parent->find(key, local);
    return symbol_type { };
  }
  void push_back(symbol_type *symbol) {
    if (!first_symbol)
      first_symbol = symbol;
    auto tmpLast = last_symbol;
    last_symbol = symbol;
    if (tmpLast) {
      last_symbol->prev = tmpLast;
      tmpLast->next = last_symbol;
    }
  }
  void push_front(symbol_type *symbol) {
    if (!last_symbol)
      last_symbol = symbol;
    auto tmpFirst = first_symbol;
    first_symbol = symbol;
    if (tmpFirst) {
      first_symbol->next = tmpFirst;
      tmpFirst->prev = first_symbol;
    }
  }
  iterator erase(iterator it) {
    symbol_type symbol = it.get();
    if (symbol) {
      auto prev = symbol.prev;
      auto next = symbol.next;
      if (prev)
        prev->next = next;
      else
        first_symbol = next;
      if (next) {
        next->prev = prev;
        return iterator { *next };
      } else
        last_symbol = prev;
      if (prev)
        return iterator { *prev };
    }
    return iterator { };
  }
  inline bool& visit() {
    return visitContext;
  }
  inline bool visit() const {
    return visitContext;
  }
  inline NodeType*& getTopContext() {
    return rootContext;
  }
  inline const NodeType* getTopContext() const {
    return rootContext;
  }
protected:
  NodeType *rootContext { };
  virtual symbol_type search(const key_type &key) const = 0;
  UniversalSymbolTable *parent { };
  symbol_type *first_symbol { };
  symbol_type *last_symbol { };
  bool visitContext = true;
};
}

#endif
