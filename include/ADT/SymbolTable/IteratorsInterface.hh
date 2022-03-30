#ifndef __SYMBOL_TABLE_ITERATORS_INTERFACE_HH__
#define __SYMBOL_TABLE_ITERATORS_INTERFACE_HH__

#include <cstddef>
#include <iterator>

namespace tlang::symbol_table::interface {
template <typename Iterator, typename Symbol>
class ForwardSymbolIterator {
public:
  using iterator_type = Iterator;
  using symbol_type = Symbol;
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = symbol_type;
  using pointer = symbol_type*;
  using reference = symbol_type&;
  ForwardSymbolIterator(symbol_type *symbol) :
      symbol(symbol) {
  }
  inline operator bool() const {
    return symbol;
  }
  inline reference operator*() {
    return *symbol;
  }
  inline const reference operator*() const {
    return *symbol;
  }
  inline pointer operator->() {
    return symbol;
  }
  inline iterator_type& getDerived() {
    return *static_cast<iterator_type*>(this);
  }
  inline const iterator_type& getDerived() const {
    return *static_cast<const iterator_type*>(this);
  }
  inline iterator_type& operator++() {
    getDerived().next();
    return getDerived();
  }
  inline iterator_type operator++(int) {
    iterator_type tmp = getDerived();
    ++(*this);
    return tmp;
  }
  inline friend bool operator==(const iterator_type &a, const iterator_type &b) {
    return a.symbol == b.symbol;
  }
  inline friend bool operator!=(const iterator_type &a, const iterator_type &b) {
    return a.symbol != b.symbol;
  }
protected:
  symbol_type *symbol { };
};
template <typename Iterator, typename Symbol>
class BidirectionalSymbolIterator: public ForwardSymbolIterator<Iterator, Symbol> {
public:
  using iterator_type = Iterator;
  using symbol_type = Symbol;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = symbol_type;
  using pointer = symbol_type*;
  using reference = symbol_type&;
  using ForwardSymbolIterator<Iterator, Symbol>::ForwardSymbolIterator;
  inline iterator_type& getDerived() {
    return *static_cast<iterator_type*>(this);
  }
  inline const iterator_type& getDerived() const {
    return *static_cast<const iterator_type*>(this);
  }
  inline iterator_type& operator--() {
    getDerived().prev();
    return getDerived();
  }
  inline iterator_type operator--(int) {
    iterator_type tmp = getDerived();
    --(*this);
    return tmp;
  }
};
template <typename ASTSymbol>
class OrderedSymbolIteratorInterface: public BidirectionalSymbolIterator<OrderedSymbolIteratorInterface<ASTSymbol>, ASTSymbol> {
public:
  using parent_type = BidirectionalSymbolIterator<OrderedSymbolIteratorInterface<ASTSymbol>, ASTSymbol>;
  using symbol_type = ASTSymbol;
  using parent_type::BidirectionalSymbolIterator;
  inline void next() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->next;
  }
  inline void prev() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->prev;
  }
};
template <typename ASTSymbol>
class ReverseOrderedSymbolIteratorInterface: public BidirectionalSymbolIterator<ReverseOrderedSymbolIteratorInterface<ASTSymbol>, ASTSymbol> {
public:
  using parent_type = BidirectionalSymbolIterator<ReverseOrderedSymbolIteratorInterface<ASTSymbol>, ASTSymbol>;
  using symbol_type = ASTSymbol;
  using parent_type::BidirectionalSymbolIterator;
  inline void next() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->prev;
  }
  inline void prev() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->next;
  }
};
template <typename ASTSymbol>
class OverloadSymbolIteratorInterface: public ForwardSymbolIterator<OverloadSymbolIteratorInterface<ASTSymbol>, ASTSymbol> {
public:
  using parent_type = ForwardSymbolIterator<OverloadSymbolIteratorInterface<ASTSymbol>, ASTSymbol>;
  using symbol_type = ASTSymbol;
  using parent_type::ForwardSymbolIterator;
  inline void next() {
    if (parent_type::symbol)
      parent_type::symbol = parent_type::symbol->overload;
  }
};
}
#endif
