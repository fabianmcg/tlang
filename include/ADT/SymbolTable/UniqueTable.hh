#ifndef ADT_SYMBOLTABLE_UNIQUETABLE_HH
#define ADT_SYMBOLTABLE_UNIQUETABLE_HH

#include <iostream>
#include <map>
#include <string>
#include <ADT/SymbolTable/TableInterface.hh>

namespace tlang::symbol_table {
template <typename Key, typename NodeType>
class UniqueTable: public SymbolTableBase<UniqueTable<Key, NodeType>, Key, NodeType> {
public:
  using key_type = Key;
  using value_type = NodeType*;
  using symbol_type = ASTSymbol<NodeType>;
  using table_type = std::map<std::pair<key_type, int>, symbol_type>;
  using UniversalSymbolTable<Key, NodeType>::find;
  UniqueTable() {
    this->visitContext = false;
  }
  UniqueTable(UniqueTable&&) = default;
  ~UniqueTable() = default;
  inline size_t size() const {
    return symbols.size();
  }
  bool add(const key_type &key, value_type &&value) {
    std::pair<key_type, int> K;
    if (key.empty())
      K = { "", unnamed++ };
    else
      K = { key, 0 };
    auto &symbol = symbols[K];
    if (symbol)
      throw(std::runtime_error("Duplicated symbol: " + key));
    symbol = symbol_type { value };
    UniversalSymbolTable<Key, NodeType>::push_back(&symbol);
    return true;
  }
  void remove(const key_type &key) {
    symbols.erase(key);
  }
  inline SymbolIterator<symbol_type> find(const key_type &key) const {
    auto it = symbols.find( { key, 0 });
    if (it != symbols.end())
      return SymbolIterator<symbol_type> { it->second };
    return SymbolIterator<symbol_type> { };
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
  int unnamed { };
};
}

#endif
