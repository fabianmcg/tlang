#ifndef CODEGEN_EMITTER_HH
#define CODEGEN_EMITTER_HH

#include <deque>
#include <unordered_map>
#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/CodeEmitterContext.hh>
#include <CodeGen/Traits.hh>

namespace tlang::codegen {
class EmitterTable {
protected:
  friend class Emitter;
  using table_type = std::unordered_map<ASTNode*, llvm::Value*>;
  inline EmitterTable(std::deque<table_type> *value_table = nullptr) :
      value_table(value_table) {
    if (value_table)
      pushContext();
  }
public:
  EmitterTable(EmitterTable&&) = default;
  EmitterTable(const EmitterTable&) = default;
  inline llvm::Value*& get(ASTNode *node) {
    return table()[node];
  }
  inline llvm::Value*& operator[](ASTNode *node) {
    return get(node);
  }
  inline llvm::Value* find(ASTNode *node, bool local = false) {
    for (auto &table : *value_table) {
      auto it = table.find(node);
      if (it != table.end())
        return it->second;
      if (local)
        break;
    }
    return nullptr;
  }
  inline void pushContext() {
    value_table->push_front( { });
  }
  inline void popContext() {
    if (value_table->size())
      value_table->pop_front();
  }
protected:
  inline table_type& table() {
    return value_table->front();
  }
  std::deque<table_type> *value_table;
};
class Emitter: public CodeEmitterContext, public EmitterTable {
public:
  Emitter(ASTContext &ast_context, llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) :
      CodeEmitterContext(ast_context, context, builder, module) {
    EmitterTable::value_table = &value_table;
    pushContext();
  }
protected:
  std::deque<table_type> value_table;
};
}

#endif
