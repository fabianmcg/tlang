#ifndef __SYMBOL_TABLE_INTERFACE_HH__
#define __SYMBOL_TABLE_INTERFACE_HH__

#include <cstddef>
#include "IteratorsInterface.hh"

namespace tlang::symbol_table::interface {
template <typename Key, typename UniversalSymbol>
class UniversalSymbolTable {
public:
  using key_type = Key;
  using symbol_type = UniversalSymbol;
  using universal_symbol_type = symbol_type;
  using parent_type = UniversalSymbolTable<key_type, universal_symbol_type>;
  using iterator = OrderedSymbolIteratorInterface<symbol_type>;
  using const_iterator = OrderedSymbolIteratorInterface<const symbol_type>;
  using reverse_iterator = ReverseOrderedSymbolIteratorInterface<symbol_type>;
  using const_reverse_iterator = ReverseOrderedSymbolIteratorInterface<const symbol_type>;
  UniversalSymbolTable() = default;
  virtual ~UniversalSymbolTable() = default;
  inline parent_type*& getParent() {
    return parent;
  }
  inline parent_type* getParent() const {
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
  inline universal_symbol_type find(const key_type &key, bool local) const {
    universal_symbol_type symbol = search(key);
    if (symbol)
      return symbol;
    if (!local && parent)
      return parent->find(key, local);
    return universal_symbol_type { };
  }
  inline bool& visit() {
    return visitContext;
  }
  inline bool visit() const {
    return visitContext;
  }
protected:
  virtual universal_symbol_type search(const key_type &key) const = 0;
  parent_type *parent { };
  symbol_type *first_symbol { };
  symbol_type *last_symbol { };
  bool visitContext = true;
};
template <typename Derived, typename Key, typename UniversalSymbol, typename Symbol>
class SymbolTable: public UniversalSymbolTable<Key, UniversalSymbol> {
public:
  using derived_type = Derived;
  using universal_symbol_table = UniversalSymbolTable<Key, UniversalSymbol>;
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
    if (this->first_symbol)
      return static_cast<symbol_type*>(this->first_symbol)->getOrderedIterator();
    return iterator { nullptr };
  }
  inline iterator end() {
    return iterator { nullptr };
  }
  inline const_iterator begin() const {
    if (this->first_symbol)
      return static_cast<symbol_type*>(this->first_symbol)->getOrderedIterator();
    return iterator { nullptr };
  }
  inline const_iterator end() const {
    return iterator { nullptr };
  }
  inline reverse_iterator rbegin() {
    if (this->last_symbol)
      return static_cast<symbol_type*>(this->last_symbol)->getReverseOrderedIterator();
    return reverse_iterator { nullptr };
  }
  inline reverse_iterator rend() {
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rbegin() const {
    if (this->last_symbol)
      return static_cast<symbol_type*>(this->last_symbol)->getReverseOrderedIterator();
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rend() const {
    return reverse_iterator { nullptr };
  }
  size_t size() const;
  void remove(const key_type &key);
  symbol_type find(const key_type &key) const;
};
}
#endif
