#ifndef ADT_SYMBOLTABLE_BASICTABLE_HH
#define ADT_SYMBOLTABLE_BASICTABLE_HH

#include <iostream>
#include <map>
#include <string>
#include "IteratorsInterface.hh"
#include "TableInterface.hh"
#include "UniversalSymbol.hh"

namespace tlang::symbol_table {
namespace interface {
template <typename NodeType>
class BasicTable: public interface::SymbolTable<BasicTable<UniversalSymbol<NodeType>>, std::string, interface::UniversalSymbol<NodeType>> {
public:
  using parent_type = interface::SymbolTable<BasicTable<UniversalSymbol<NodeType>>, std::string, interface::UniversalSymbol<NodeType>>;
  using universal_symbol_table = typename parent_type::universal_symbol_table;
  using key_type = typename parent_type::key_type;
  using universal_symbol_type = typename parent_type::universal_symbol_type;
  using value_type = NodeType*;
  using symbol_type = UniversalSymbol<NodeType>;
  using table_type = std::map<std::pair<key_type, int>, symbol_type>;
  BasicTable() {
    this->visitContext = false;
  }
  ~BasicTable() = default;
  BasicTable(BasicTable&&) = default;
  BasicTable(const BasicTable&) = delete;
  BasicTable& operator=(BasicTable&&) = default;
  BasicTable& operator=(const BasicTable&) = delete;
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
    if (!this->first_symbol)
      this->first_symbol = &symbol;
    auto tmpLast = this->last_symbol;
    this->last_symbol = &symbol;
    if (tmpLast) {
      symbol.prev = tmpLast;
      tmpLast->next = this->last_symbol;
    }
    return true;
  }
  void remove(const key_type &key) {
    symbols.erase(key);
  }
  inline symbol_type find(const key_type &key) const {
    auto it = symbols.find( { key, 0 });
    if (it != symbols.end())
      return it->second;
    return symbol_type { };
  }
  void print(std::ostream &ost) const {
    ost << this << "[" << this->parent << "]:" << std::endl;
    for (auto& [k, v] : symbols) {
      ost << "\t" << k << ": [" << v << "]";
      ost << std::endl;
    }
  }
protected:
  virtual universal_symbol_type search(const key_type &key) const {
    return find(key);
  }
  table_type symbols { };
  int unnamed { };
};
}
template <typename NodeType>
using BasicTable = interface::BasicTable<NodeType>;
}

#endif
