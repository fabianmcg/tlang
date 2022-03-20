#ifndef __SYMBOLTABLE_UNIVERSAL_SYMBOL_HH__
#define __SYMBOLTABLE_UNIVERSAL_SYMBOL_HH__

#include "IteratorsInterface.hh"
#include <iostream>

namespace tlang::symbol_table::interface {
template <typename NodeType>
struct UniversalSymbol {
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
  inline iterator getIterator() {
    return iterator { this };
  }
  inline const_iterator getIterator() const {
    return iterator { this };
  }
  inline reversed_iterator getReversedIterator() {
    return reversed_iterator { this };
  }
  inline reversed_iterator getReversedIterator() const {
    return const_reversed_iterator { this };
  }
  NodeType *node { };
  UniversalSymbol *prev { };
  UniversalSymbol *next { };
};
}
#endif
