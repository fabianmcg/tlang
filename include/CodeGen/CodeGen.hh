#ifndef __CODEGEN_CODEGEN_HH__
#define __CODEGEN_CODEGEN_HH__

#include "AST/Include.hh"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

namespace tlang::codegen {
using namespace _astnp_;
inline void initLLVM() {
  using namespace llvm;
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
}
struct TypeVisitor: RecursiveASTVisitor<TypeVisitor, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  TypeVisitor(llvm::LLVMContext &context, std::map<Decl*, llvm::Type*> &type_translation_table) :
      context(context), type_translation_table(type_translation_table) {
  }
  visit_t visitQualType(QualType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    if (node->isReference())
      type = llvm::PointerType::get(type, 0);
    return visit_value;
  }
  visit_t visitPtrType(PtrType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::PointerType::get(type, 0);
    return visit_value;
  }
  visit_t visitIntType(IntType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::Type::getInt32Ty(context);
    return visit_value;
  }
  visit_t visitFloatType(FloatType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::Type::getFloatTy(context);
    return visit_value;
  }
  visit_t visitStructType(StructType *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    type = llvm::StructType::create(context, node->getIdentifier());
    type_translation_table[node->getDecl()] = type;
    return visit_value;
  }
  llvm::Type* make(QualType &node) {
    traverseQualType(&node);
    return type;
  }
  llvm::LLVMContext &context;
  llvm::Type *type { };
  std::map<Decl*, llvm::Type*> &type_translation_table;
};
struct CodeGenVisitor: RecursiveASTVisitor<CodeGenVisitor, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  CodeGenVisitor(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) :
      context(context), builder(builder), module(module) {
  }
  visit_t visitFunctorDecl(FunctorDecl *node, bool isFirst) {
    using namespace llvm;
    if (!node->getComplete())
      return visit_t::skip;
    if (isFirst) {
      auto &fp = node->getParameters();
      std::vector<llvm::Type*> parameters(fp.size(), nullptr);
      for (size_t i = 0; i < fp.size(); ++i) {
        parameters[i] = make(fp[i]->getType());
      }
      auto rt = make(node->getReturntype());
      llvm::FunctionType *FT = llvm::FunctionType::get(rt, parameters, false);
      Function *F = Function::Create(FT, Function::ExternalLinkage, node->getIdentifier(), &module);
      unsigned Idx = 0;
      for (auto &Arg : F->args())
        Arg.setName(fp[Idx++]->getIdentifier());
      BasicBlock *BB = BasicBlock::Create(context, "entry", F);
      builder.SetInsertPoint(BB);
    }
    return visit_value;
  }
  visit_t visitStructDecl(StructDecl *node, bool isFirst) {
    if (isFirst) {
      std::vector<llvm::Type*> parameters;
      for (auto child : **static_cast<DeclContext*>(node))
        if (child->is(NodeClass::MemberDecl)) {
          MemberDecl *decl = dynamic_cast<MemberDecl*>(child);
          parameters.push_back(make(decl->getType()));
        }
      llvm::StructType *Struct;
      auto &type = type_translation_table[node];
      if (!type) {
        Struct = llvm::StructType::create(context, node->getIdentifier());
        type = Struct;
      } else
        Struct = llvm::cast<llvm::StructType>(type);
      Struct->setBody(parameters);
    }
    return visit_value;
  }
  llvm::Type* make(QualType &node) {
    return TypeVisitor { context, type_translation_table }.make(node);
  }
  llvm::LLVMContext &context;
  llvm::IRBuilder<> &builder;
  llvm::Module &module;
  std::map<ASTNode*, llvm::Value*> translation_table;
  std::map<Decl*, llvm::Type*> type_translation_table;
};
class CodeGen {
public:
  CodeGen(const std::string &module_name) {
    init(module_name);
  }
  void init(const std::string &module_name) {
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
  void gen(ModuleDecl *m) {
    CodeGenVisitor { *context, *builder, *module }.traverseModuleDecl(m);
  }
  void print(std::ostream &ost) {
    std::string code;
    llvm::raw_string_ostream rost(code);
    module->print(rost, nullptr);
    ost << code;
  }
protected:
  std::unique_ptr<llvm::LLVMContext> context { };
  std::unique_ptr<llvm::IRBuilder<>> builder { };
  std::unique_ptr<llvm::Module> module { };
};
}
#endif
