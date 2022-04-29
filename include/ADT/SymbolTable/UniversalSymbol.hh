#ifndef ADT_SYMBOLTABLE_UNIVERSALSYMBOL_HH
#define ADT_SYMBOLTABLE_UNIVERSALSYMBOL_HH

#include <cassert>
#include <iterator>

namespace tlang::symbol_table {
template <typename NodeType>
class UniversalSymbol {
public:
  template <typename Key, typename UniversalSymbol>
  friend class UniversalSymbolTable;
  template <typename, typename, typename, typename >
  friend class SymbolTableBase;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = NodeType;
  using pointer = value_type*;
  using reference = value_type&;
  UniversalSymbol() = default;
  inline UniversalSymbol(NodeType *node) :
      node(node) {
  }
  inline UniversalSymbol(NodeType *node, UniversalSymbol *prev, UniversalSymbol *next) :
      node(node), prev(prev), next(next) {
  }
  inline operator bool() const {
    return node;
  }
  inline pointer operator*() {
    return node;
  }
  inline const pointer operator*() const {
    return node;
  }
  inline pointer operator->() {
    return node;
  }
  inline const pointer operator->() const {
    return node;
  }
  inline UniversalSymbol& operator++() {
    if (next)
      *this = *next;
    else
      *this = makeEnd();
    return *this;
  }
  inline UniversalSymbol operator++(int) {
    UniversalSymbol tmp = *this;
    ++(*this);
    return tmp;
  }
  inline UniversalSymbol& operator--() {
    if (prev)
      *this = *prev;
    else
      *this = makeEnd();
    return *this;
  }
  inline UniversalSymbol operator--(int) {
    UniversalSymbol tmp = *this;
    --(*this);
    return tmp;
  }
  inline friend bool operator==(const UniversalSymbol &a, const UniversalSymbol &b) {
    return a.node == b.node;
  }
  inline friend bool operator!=(const UniversalSymbol &a, const UniversalSymbol &b) {
    return a.node != b.node;
  }
  template <typename T = NodeType>
  inline T* get() {
    return static_cast<T*>(node);
  }
protected:
  pointer node { };
  UniversalSymbol *prev { };
  UniversalSymbol *next { };
  UniversalSymbol makeEnd() const {
    return UniversalSymbol { nullptr, const_cast<UniversalSymbol*>(this), nullptr };
  }
  UniversalSymbol makeREnd() const {
    return UniversalSymbol { nullptr, nullptr, const_cast<UniversalSymbol*>(this) };
  }
};
}
#endif
