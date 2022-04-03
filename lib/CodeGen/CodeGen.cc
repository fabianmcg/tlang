#include <CodeGen/CodeGen.hh>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"

namespace tlang::codegen {
static void initLLVM() {
  using namespace llvm;
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}
void CodeGen::init() {
  using namespace llvm;
  llvm_context = std::make_unique<LLVMContext>();
  initLLVM();
}
void CodeGen::initEmitters() {

}
void CodeGen::emit(UnitDecl *unit, llvm::raw_ostream &ost) {
  auto CGunit = makeUnit(unit);
  CGunit.emit(ost);
}
CodeGenUnit CodeGen::makeUnit(UnitDecl *unit) {
  return CodeGenUnit(*emitters.at(unit->getGenKind()), *unit, ast_context, *llvm_context);
}
}
