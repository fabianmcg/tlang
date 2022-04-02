#include <CodeGen/CodeGenContext.hh>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

namespace tlang::codegen {
static void initLLVM() {
  using namespace llvm;
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}
void CodeGenContext::init(const std::string &module_name) {
  using namespace llvm;
  context = std::make_unique<LLVMContext>();
  module = std::make_unique<Module>(module_name, *context);
  builder = std::make_unique<IRBuilder<>>(*context);
  initLLVM();

  auto target_triple = sys::getDefaultTargetTriple();
  module->setTargetTriple(target_triple);
  std::string error;
  auto target = TargetRegistry::lookupTarget(target_triple, error);
  if (!target) {
    throw(std::runtime_error("Code generation error: " + error));
  }
  auto CPU = "generic";
  auto Features = "";
  TargetOptions opt;
  auto RM = Optional<Reloc::Model>();
  auto target_machine = target->createTargetMachine(target_triple, CPU, Features, opt, RM);
  module->setDataLayout(target_machine->createDataLayout());
}
//void CodeGen::generate(ModuleDecl *module) {
//  CGContext ctx = makeContext();
//  ctx.emitModuleDecl(module);
//}
//void CodeGen::emit(ModuleDecl *m, std::ostream &ost) {
//  generate(m);
//  print(ost);
//}
void CodeGenContext::print(std::ostream &ost) {
  std::string code;
  llvm::raw_string_ostream rost(code);
  module->print(rost, nullptr);
  ost << code;
}
}
