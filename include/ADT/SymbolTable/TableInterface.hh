#ifndef ADT_SYMBOLTABLE_TABLEINTERFACE_HH
#define ADT_SYMBOLTABLE_TABLEINTERFACE_HH

#include <cstddef>
#include "UniversalTable.hh"

namespace tlang::symbol_table::interface {
template <typename Derived, typename Key, typename UniversalSymbol, typename Symbol = UniversalSymbol>
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
  using universal_symbol_table::find;
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
private:
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
};
}

#endif
