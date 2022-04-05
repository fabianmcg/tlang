#ifndef ADT_SYMBOLTABLE_ASTSYMBOL_HH
#define ADT_SYMBOLTABLE_ASTSYMBOL_HH

#include <ADT/SymbolTable/UniversalTable.hh>

namespace tlang::symbol_table {
template <typename NodeType>
class ASTSymbol: public UniversalSymbol<NodeType> {
public:
  template <typename, typename, typename, typename >
  friend class SymbolTableBase;
  template <typename, typename >
  friend class ASTSymbolTable;
  using value_type = NodeType;
  using pointer = value_type*;
  using reference = value_type&;
  ASTSymbol() = default;
  inline ASTSymbol(NodeType *node) :
      UniversalSymbol<NodeType>(node) {
  }
  inline ASTSymbol(NodeType *node, ASTSymbol *prev, ASTSymbol *next, ASTSymbol *overload) :
      UniversalSymbol<NodeType>(node, prev, next), overload(overload) {
  }
  inline ASTSymbol& operator++() {
    if (this->next)
      *this = *static_cast<ASTSymbol*>(this->next);
    else
      *this = makeEnd();
    return *this;
  }
  inline ASTSymbol operator++(int) {
    ASTSymbol tmp = *this;
    ++(*this);
    return tmp;
  }
  inline ASTSymbol& operator--() {
    if (this->prev)
      *this = *static_cast<ASTSymbol*>(this->prev);
    else
      *this = makeEnd();
    return *this;
  }
  inline ASTSymbol operator--(int) {
    ASTSymbol tmp = *this;
    --(*this);
    return tmp;
  }
  inline void print(std::ostream &ost) const {
    ost << this->node << ": " << this->prev << " -> " << this->next << " |-> " << overload;
  }
  inline friend std::ostream& operator<<(std::ostream &ost, const ASTSymbol &symbol) {
    symbol.print(ost);
    return ost;
  }
protected:
  ASTSymbol *overload { };
  ASTSymbol makeEnd() const {
    return ASTSymbol { nullptr, const_cast<ASTSymbol*>(this), nullptr, nullptr };
  }
  ASTSymbol makeREnd() const {
    return ASTSymbol { nullptr, nullptr, const_cast<ASTSymbol*>(this), nullptr };
  }
};
}

#endif
