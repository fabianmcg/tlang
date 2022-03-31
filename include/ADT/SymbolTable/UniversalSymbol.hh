#ifndef ADT_SYMBOLTABLE_UNIVERSALSYMBOL_HH
#define ADT_SYMBOLTABLE_UNIVERSALSYMBOL_HH

#include "IteratorsInterface.hh"
#include <iostream>

namespace tlang::symbol_table::interface {
template <typename NodeType>
class UniversalSymbol {
public:
  using value_type = NodeType;
  using pointer = value_type*;
  using iterator = OrderedSymbolIteratorInterface<UniversalSymbol>;
  using const_iterator = OrderedSymbolIteratorInterface<const UniversalSymbol>;
  using reversed_iterator = ReverseOrderedSymbolIteratorInterface<UniversalSymbol>;
  using const_reversed_iterator = ReverseOrderedSymbolIteratorInterface<const UniversalSymbol>;
  UniversalSymbol() = default;
  inline UniversalSymbol(NodeType *node) :
      node(node) {
  }
  inline operator bool() const {
    return node;
  }
  inline NodeType*& operator*() {
    return node;
  }
  inline NodeType* operator*() const {
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
  inline iterator getOrderedIterator() {
    return iterator { this };
  }
  inline const_iterator getOrderedIterator() const {
    return iterator { this };
  }
  inline reversed_iterator getReverseOrderedIterator() {
    return reversed_iterator { this };
  }
  inline const_reversed_iterator getReverseOrderedIterator() const {
    return const_reversed_iterator { this };
  }
  template <typename T = UniversalSymbol>
  T* getNext() {
    return static_cast<T*>(next);
  }
  template <typename T = UniversalSymbol>
  T* getPrev() {
    return static_cast<T*>(prev);
  }
  pointer node { };
  UniversalSymbol *prev { };
  UniversalSymbol *next { };
};
}
#endif
