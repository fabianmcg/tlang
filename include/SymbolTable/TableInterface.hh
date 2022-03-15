#ifndef __SYMBOL_TABLE_INTERFACE_HH__
#define __SYMBOL_TABLE_INTERFACE_HH__

#include <cstddef>
#include "IteratorsInterface.hh"

namespace tlang::symbol_table::interface {
template <typename K, typename V>
class UniversalSymbolTable {
public:
  using key_type = K;
  using universal_symbol_type = V;
  using parent_type = UniversalSymbolTable<key_type, universal_symbol_type>;
  virtual ~UniversalSymbolTable() = default;
  inline parent_type*& getParent() {
    return parent;
  }
  inline parent_type* getParent() const {
    return parent;
  }
  inline universal_symbol_type find(const key_type &key, bool local) const {
    universal_symbol_type symbol = search(key);
    if (symbol)
      return symbol;
    if (!local && parent)
      return parent->find(key, local);
    return universal_symbol_type { };
  }
protected:
  virtual universal_symbol_type search(const key_type &key) const = 0;
  parent_type *parent { };
};
template <typename Derived, typename Key, typename UniversalSymbol, typename Symbol>
class SymbolTable: public UniversalSymbolTable<Key, UniversalSymbol> {
public:
  using universal_symbol_table = UniversalSymbolTable<Key, UniversalSymbol>;
  using derived_type = Derived;
  using key_type = Key;
  using universal_symbol_type = UniversalSymbol;
  using symbol_type = Symbol;
  using iterator = OrderedSymbolIteratorInterface<symbol_type>;
  using const_iterator = OrderedSymbolIteratorInterface<const symbol_type>;
  using reverse_iterator = ReverseOrderedSymbolIteratorInterface<symbol_type>;
  using const_reverse_iterator = ReverseOrderedSymbolIteratorInterface<const symbol_type>;
  inline derived_type& operator*() {
    return getDerived();
  }
  inline const derived_type& operator*() const {
    return getDerived();
  }
  inline derived_type& getDerived() {
    return *static_cast<derived_type*>(this);
  }
  inline const derived_type& getDerived() const {
    return *static_cast<const derived_type*>(this);
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
  inline bool& visit() {
    return visitCtxt;
  }
  inline bool visit() const {
    return visitCtxt;
  }
  size_t size() const;
  void remove(const key_type &key);
  symbol_type find(const key_type &key) const;
protected:
  symbol_type *first_symbol { };
  symbol_type *last_symbol { };
  bool visitCtxt = true;
};
}
#endif
