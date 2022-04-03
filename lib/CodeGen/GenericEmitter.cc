#include <CodeGen/GenericEmitter.hh>

namespace tlang::codegen {
std::map<ASTNode*, llvm::Value*>& GenericEmitter::values() {
  return valueTable.front();
}
void GenericEmitter::pushContext() {
  valueTable.push_front( { });
}
void GenericEmitter::popContext() {
  if (valueTable.size() > 1)
    valueTable.pop_front();
}
void GenericEmitter::run(UnitDecl *unit) {
  pushContext();
  emitUnitDecl(unit);
  popContext();
}
}
