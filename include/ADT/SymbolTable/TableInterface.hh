#ifndef ADT_SYMBOLTABLE_TABLEINTERFACE_HH
#define ADT_SYMBOLTABLE_TABLEINTERFACE_HH

#include <cstddef>
#include <ADT/SymbolTable/UniversalTable.hh>

namespace tlang::symbol_table {
template <typename Derived, typename Key, typename NodeType, typename Symbol = UniversalSymbol<NodeType>>
class SymbolTableBase: public UniversalSymbolTable<Key, NodeType> {
public:
  using derived_type = Derived;
  using universal_symbol_table = UniversalSymbolTable<Key, NodeType>;
  using key_type = Key;
  using universal_symbol_type = UniversalSymbol<NodeType>;
  using symbol_type = Symbol;
  using iterator = SymbolIterator<symbol_type>;
  using const_iterator = SymbolIterator<symbol_type, true>;
  using reverse_iterator = SymbolIterator<symbol_type, false, true>;
  using const_reverse_iterator = SymbolIterator<symbol_type, true, true>;
  using UniversalSymbolTable<Key, NodeType>::UniversalSymbolTable;
  using universal_symbol_table::find;
  inline iterator begin() {
    if (this->first_symbol)
      return iterator { *getFirst() };
    return iterator { };
  }
  inline iterator end() {
    if (this->last_symbol)
      return iterator { getLast()->makeEnd() };
    return iterator { };
  }
  inline const_iterator begin() const {
    if (this->first_symbol)
      return const_iterator { *getFirst() };
    return const_iterator { };
  }
  inline const_iterator end() const {
    if (this->last_symbol)
      return const_iterator { getLast()->makeEnd() };
    return const_iterator { };
  }
  inline reverse_iterator rbegin() {
    if (this->last_symbol)
      return reverse_iterator { *getLast() };
    return reverse_iterator { };
  }
  inline reverse_iterator rend() {
    if (this->first_symbol)
      return reverse_iterator { getFirst()->makeREnd() };
    return reverse_iterator { };
  }
  inline const_reverse_iterator rbegin() const {
    if (this->last_symbol)
      return const_reverse_iterator { *getLast() };
    return reverse_iterator { nullptr };
  }
  inline const_reverse_iterator rend() const {
    if (this->first_symbol)
      return const_reverse_iterator { getFirst()->makeREnd() };
    return const_reverse_iterator { };
  }
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
  inline symbol_type* getFirst() {
    return static_cast<symbol_type*>(this->first_symbol);
  }
  inline const symbol_type* getFirst() const {
    return static_cast<const symbol_type*>(this->first_symbol);
  }
  inline symbol_type* getLast() {
    return static_cast<symbol_type*>(this->last_symbol);
  }
  inline const symbol_type* getLast() const {
    return static_cast<const symbol_type*>(this->last_symbol);
  }
};
}

#endif
