#ifndef CODEGEN_EMITTERCONTEXT_HH
#define CODEGEN_EMITTERCONTEXT_HH

#include <deque>
#include <unordered_map>
#include <AST/ASTNode.hh>
#include <CodeGen/LLVMFwdTypes.hh>

namespace tlang::codegen {
class EmitterContext {
public:
  using table_type = std::unordered_map<ASTNode*, llvm::Value*>;
  inline EmitterContext() {
    pushContext();
  }
  inline llvm::Value*& get(ASTNode *node) {
    return table()[node];
  }
  inline llvm::Value*& operator[](ASTNode *node) {
    return get(node);
  }
  inline llvm::Value* find(ASTNode *node, bool local = false) {
    for (auto &table : value_table) {
      auto it = table.find(node);
      if (it != table.end())
        return it->second;
      if (local)
        break;
    }
    return nullptr;
  }
  inline void pushContext() {
    value_table.push_front( { });
  }
  inline void popContext() {
    if (value_table.size())
      value_table.pop_front();
  }
protected:
  inline table_type& table() {
    return value_table.front();
  }
  std::deque<table_type> value_table;
};
}
#endif
