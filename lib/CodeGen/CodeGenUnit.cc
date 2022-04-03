#include <CodeGen/CodeGenUnit.hh>
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

namespace tlang::codegen {
namespace {
void initGenericTarget(llvm::Module *module) {
  auto target_triple = llvm::sys::getDefaultTargetTriple();
  module->setTargetTriple(target_triple);
  std::string error;
  auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);
  if (!target) {
    throw(std::runtime_error("Code generation error: " + error));
  }
  auto CPU = "generic";
  auto Features = "";
  llvm::TargetOptions opt;
  auto RM = llvm::Optional<llvm::Reloc::Model>();
  auto target_machine = target->createTargetMachine(target_triple, CPU, Features, opt, RM);
  module->setDataLayout(target_machine->createDataLayout());
}
}
CodeGenUnit::CodeGenUnit(UnitDecl *unit, ASTContext &ast_context, llvm::LLVMContext &llvm_context) :
    unit(unit), ast_context(ast_context), llvm_context(llvm_context) {
  init();
}
void CodeGenUnit::emit(CodeEmitter &emitter, llvm::raw_ostream &ost) {
  emitter.run(unit);
  if (module)
    module->print(ost, nullptr);
}
void CodeGenUnit::init() {
  if (unit && !module && !builder) {
    module = std::make_unique<llvm::Module>(unit->getIdentifier(), llvm_context);
    builder = std::make_unique<llvm::IRBuilder<>>(llvm_context);
    if (unit->getGenKind() < UnitDecl::NVPTX)
      initGenericTarget(module.get());
  }
}
}
