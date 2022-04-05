#ifndef ADT_SYMBOLTABLE_ITERATORS_HH
#define ADT_SYMBOLTABLE_ITERATORS_HH

#include <cstddef>
#include <iterator>

namespace tlang::symbol_table {
template <typename SymbolType, bool isConst = false, bool reverse = false>
class SymbolIterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = typename SymbolType::value_type;
  using pointer = value_type*;
  using reference = value_type&;
  SymbolIterator() = default;
  SymbolIterator(const SymbolType &symbol) :
      symbol(symbol) {
  }
  inline operator bool() const {
    return symbol;
  }
  inline std::conditional_t<isConst, const pointer, pointer> operator*() {
    return *symbol;
  }
  inline const pointer operator*() const {
    return *symbol;
  }
  inline std::conditional_t<isConst, const pointer, pointer> operator->() {
    return symbol.operator->();
  }
  inline const pointer operator->() const {
    return symbol.operator->();
  }
  inline SymbolIterator& operator++() {
    if constexpr (reverse)
      --symbol;
    else
      ++symbol;
    return *this;
  }
  inline SymbolIterator operator++(int) {
    SymbolIterator tmp = *this;
    ++(*this);
    return tmp;
  }
  inline SymbolIterator operator--() {
    if constexpr (reverse)
      ++symbol;
    else
      --symbol;
    return *this;
  }
  inline auto operator--(int) {
    SymbolIterator tmp = *this;
    --(*this);
    return tmp;
  }
  inline friend bool operator==(const SymbolIterator &a, const SymbolIterator &b) {
    return a.symbol == b.symbol;
  }
  inline friend bool operator!=(const SymbolIterator &a, const SymbolIterator &b) {
    return a.symbol != b.symbol;
  }
  SymbolType get() {
    return symbol;
  }
protected:
  SymbolType symbol { };
};
}

#endif
