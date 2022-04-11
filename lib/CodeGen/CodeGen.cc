#include <CodeGen/CodeGen.hh>
#include <CodeGen/GenericEmitter.hh>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

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
void initNVPTXTarget(llvm::Module *module) {
  auto target_triple = "nvptx64-nvidia-cuda";
  module->setTargetTriple(target_triple);
  std::string error;
  auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);
  if (!target) {
    throw(std::runtime_error("Code generation error: " + error));
  }
  auto CPU = "sm_35";
  auto Features = "+ptx50";
  llvm::TargetOptions opt;
  auto RM = llvm::Optional<llvm::Reloc::Model>();
  auto target_machine = target->createTargetMachine(target_triple, CPU, Features, opt, RM);
  module->setDataLayout(target_machine->createDataLayout());
}
std::unique_ptr<llvm::Module> makeModule(UnitDecl *unit, llvm::LLVMContext &context) {
  std::unique_ptr<llvm::Module> module;
  module = std::make_unique<llvm::Module>(unit->getIdentifier(), context);
  if (unit->getGenKind() < UnitDecl::NVPTX)
    initGenericTarget(module.get());
  if (unit->getGenKind() == UnitDecl::NVPTX)
    initNVPTXTarget(module.get());
  return module;
}
}
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
llvm::Module& CodeGen::getModule(UnitDecl *unit) {
  auto &module = modules[unit->getIdentifier()];
  if (!module)
    module = makeModule(unit, *llvm_context);
  return *module;
}
llvm::Module* CodeGen::emit(UnitDecl *unit) {
  assert(unit);
  llvm::IRBuilder<> builder { *llvm_context };
  auto &module = getModule(unit);
  if (unit->getGenKind() <= UnitDecl::NVPTX) {
    GenericEmitter emitter = makeEmitter<GenericEmitter>(ast_context, *llvm_context, module, builder);
    emitter.run(unit);
  }
  return &module;
}
}
