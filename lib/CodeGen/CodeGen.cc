#include <CodeGen/CodeGen.hh>
#include <CodeGen/GenericEmitter.hh>
#include <CodeGen/CXXEmitter.hh>
#include <Support/Format.hh>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <CodeGen/NVPTXEmitter.hh>

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
  if (unit->getBackend() < UnitDecl::NVPTX)
    initGenericTarget(module.get());
  if (unit->getBackend() == UnitDecl::NVPTX)
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
  if (unit->getBackend() <= UnitDecl::NVPTX) {
    GenericEmitter emitter = makeEmitter<GenericEmitter>(*compiler_context, *llvm_context, module, builder);
    for (auto module : *unit)
      emitter.emitModuleDecl(dyn_cast<ModuleDecl>(module));
  }
  return &module;
}
void CodeGen::emit(UnitDecl *unit, llvm::raw_ostream &ost) {
  assert(unit);
  if (unit->getBackend() != Backend::CXX) {
    llvm::IRBuilder<> builder { *llvm_context };
    auto &module = getModule(unit);
    if (unit->getBackend() < UnitDecl::NVPTX) {
      GenericEmitter emitter = makeEmitter<GenericEmitter>(*compiler_context, *llvm_context, module, builder);
      emitter.emitForwardDecl(unit);
      for (auto module : *unit)
        emitter.emitModuleDecl(dyn_cast<ModuleDecl>(module));
    } else if (unit->getBackend() == UnitDecl::NVPTX) {
      NVPTXEmitter emitter = makeEmitter<NVPTXEmitter>(*compiler_context, *llvm_context, module, builder);
      emitter.emitForwardDecl(unit);
      for (auto module : *unit)
        emitter.emitModuleDecl(dyn_cast<ModuleDecl>(module));
    }
    module.print(ost, nullptr);
  } else {
    for (auto module : *unit)
      CXXEmitter { }.emitModuleDecl(dyn_cast<ModuleDecl>(module), ost);
  }
}
int CodeGen::emit(std::filesystem::path output) {
  std::filesystem::path path = std::filesystem::absolute(output).parent_path();
  std::filesystem::path basename = output.stem();
  for (auto decl : (***compiler_context)) {
    auto unit = static_cast<UnitDecl*>(decl);
    auto file = path;
    switch (unit->getBackend()) {
    case Backend::AMDGPU:
    case Backend::NVPTX:
      file /= basename;
      file += std::filesystem::path(".device.ll");
      break;
    case Backend::CXX:
      file /= basename;
      file += std::filesystem::path(".rt.cpp");
      break;
    default:
      file /= output.filename();
      break;
    }
    std::cerr << "Emitting: " << file << std::endl;
    std::error_code code;
    llvm::raw_fd_ostream os(file.string(), code);
    if (code) {
      std::cerr << code.message() << std::endl;
      return 1;
    }
    emit(unit, os);
    std::cerr << "Finished emitting: " << file << std::endl;
  }
  return 0;
}
}
