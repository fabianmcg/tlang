#include <CodeGen/CodeGen.hh>
#include <CodeGen/GenericEmitter.hh>
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
void CodeGen::emit(UnitDecl *unit, llvm::raw_ostream &ost) {
  assert(unit);
  auto CGunit = makeUnit(unit);
  if (unit->getGenKind() < UnitDecl::NVPTX) {
    GenericEmitter emitter = CGunit.makeEmitter<GenericEmitter>();
    CGunit.emit(emitter, ost);
  }
}
CodeGenUnit CodeGen::makeUnit(UnitDecl *unit) {
  return CodeGenUnit(unit, ast_context, *llvm_context);
}
}
