#ifndef ADT_SYMBOLTABLE_SYMBOLTABLE_HH
#define ADT_SYMBOLTABLE_SYMBOLTABLE_HH

#include <iostream>
#include <map>
#include <string>
#include <ADT/SymbolTable/ASTSymbol.hh>
#include <ADT/SymbolTable/TableInterface.hh>

namespace tlang::symbol_table {
template <typename Key, typename NodeType>
class ASTSymbolTable: public SymbolTableBase<ASTSymbolTable<Key, NodeType>, Key, NodeType, ASTSymbol<NodeType>> {
public:
  using key_type = Key;
  using value_type = NodeType*;
  using symbol_type = ASTSymbol<NodeType>;
  using table_type = std::multimap<key_type, symbol_type>;
  using iterator = SymbolIterator<symbol_type>;
  using UniversalSymbolTable<Key, NodeType>::find;
  ASTSymbolTable() = default;
  inline size_t size() const {
    return symbols.size();
  }
  bool add(const key_type &key, value_type &&value) {
    auto it = symbols.insert( { key, symbol_type { value } });
    auto &symbol = it->second;
    UniversalSymbolTable<Key, NodeType>::push_back(&symbol);
    if (it != symbols.begin())
      if ((--it)->first == key)
        it->second.overload = &symbol;
    return true;
  }
  void remove(const key_type &key) {
    symbols.erase(key);
  }
  iterator erase(iterator it) {
    if (it) {
      UniversalSymbolTable<Key, NodeType>::erase(it.get());
    }
    return iterator{};
  }
  inline iterator find(const key_type &key) const {
    auto it = symbols.find(key);
    if (it != symbols.end())
      return iterator { it->second };
    return iterator { };
  }
  void print(std::ostream &ost) const {
    ost << this << "[" << this->parent << "]:" << std::endl;
    for (auto& [k, v] : symbols) {
      ost << "\t" << k << ": [" << v << "]";
      ost << std::endl;
    }
  }
protected:
  virtual UniversalSymbol<NodeType> search(const key_type &key) const {
    return find(key).get();
  }
  table_type symbols { };
};
}

#endif
