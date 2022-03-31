#ifndef ADT_SYMBOLTABLE_UNIVERSALTABLE_HH
#define ADT_SYMBOLTABLE_UNIVERSALTABLE_HH

#include "IteratorsInterface.hh"

namespace tlang::symbol_table::interface {
template <typename Key, typename UniversalSymbol>
class UniversalSymbolTable {
public:
  using key_type = Key;
  using symbol_type = UniversalSymbol;
  using iterator = OrderedSymbolIteratorInterface<symbol_type>;
  using const_iterator = OrderedSymbolIteratorInterface<const symbol_type>;
  using reverse_iterator = ReverseOrderedSymbolIteratorInterface<symbol_type>;
  using const_reverse_iterator = ReverseOrderedSymbolIteratorInterface<const symbol_type>;
  UniversalSymbolTable() = default;
  virtual ~UniversalSymbolTable() = default;
  inline UniversalSymbolTable*& getParent() {
    return parent;
  }
  inline UniversalSymbolTable* getParent() const {
    return parent;
  }
  inline iterator begin() {
    if (first_symbol)
      return first_symbol->getOrderedIterator();
    return iterator { nullptr };
  }
  inline iterator end() {
    return iterator { nullptr };
  }
  inline const_iterator begin() const {
    if (first_symbol)
      return first_symbol->getOrderedIterator();
    return iterator { nullptr };
  }
  inline const_iterator end() const {
    return iterator { nullptr };
  }
  inline reverse_iterator rbegin() {
    if (last_symbol)
      return last_symbol->getReverseOrderedIterator();
    return reverse_iterator { nullptr };
  }
  inline reverse_iterator rend() {
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rbegin() const {
    if (last_symbol)
      return last_symbol->getReverseOrderedIterator();
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rend() const {
    return reverse_iterator { nullptr };
  }
  inline symbol_type find(const key_type &key, bool local) const {
    symbol_type symbol = search(key);
    if (symbol)
      return symbol;
    if (!local && parent)
      return parent->find(key, local);
    return symbol_type { };
  }
  inline bool& visit() {
    return visitContext;
  }
  inline bool visit() const {
    return visitContext;
  }
protected:
  virtual symbol_type search(const key_type &key) const = 0;
  UniversalSymbolTable *parent { };
  symbol_type *first_symbol { };
  symbol_type *last_symbol { };
  bool visitContext = true;
};
}

#endif
