#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Signals.h>
#include <llvm/TableGen/Error.h>
#include <llvm/TableGen/Main.h>
#include <Utility.hh>
#include <Emitters/Emitters.hh>

llvm::cl::OptionCategory TableGenCMDCat = llvm::cl::OptionCategory("TableGen");

void fixOptions() {
  auto opts = llvm::cl::getRegisteredOptions();
  if (auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(opts.lookup("o")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(opts.lookup("d")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(opts.lookup("I")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(opts.lookup("D")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<bool>*>(opts.lookup("write-if-changed")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<bool>*>(opts.lookup("time-phases")))
    opt->addCategory(TableGenCMDCat);
  if (auto opt = dynamic_cast<llvm::cl::opt<bool>*>(opts.lookup("no-warn-on-unused-template-args")))
    opt->addCategory(TableGenCMDCat);
}

void epilogue() {
  auto opts = llvm::cl::getRegisteredOptions();
  if (auto opt = dynamic_cast<llvm::cl::opt<std::string>*>(opts.lookup("o")))
    if (opt->getValue() != "-")
      formatFile(opt->getValue());
}

enum ActionType {
  PrintRecords,
  DumpJSON,
  emitNodes,
  emitCommon,
  emitTraits,
  emitInc,
  emitCodeGen,
};

static llvm::cl::opt<ActionType> action(
    llvm::cl::desc("Action to perform:"), llvm::cl::cat(TableGenCMDCat),
    llvm::cl::values(
        clEnumValN(PrintRecords, "print-records", "Print all records to stdout (default)"),
        clEnumValN(DumpJSON, "dump-json", "Dump all records as machine-readable JSON"),
        clEnumValN(emitCommon, "emit-common", "Emit common header"),
        clEnumValN(emitTraits, "emit-traits", "Emit traits header"),
        clEnumValN(emitNodes, "emit-nodes", "Emit AST nodes"),
        clEnumValN(emitInc, "emit-inc", "Emit INC file"),
        clEnumValN(emitCodeGen, "emit-codegen", "Emit codegen visitor file")
        ));

static llvm::cl::opt<EmitMode> emitMode(
    llvm::cl::desc("Emit mode:"), llvm::cl::cat(TableGenCMDCat),
    llvm::cl::values(
        clEnumValN(EmitMode::header, "hh", "Emit header"),
        clEnumValN(EmitMode::source, "cc", "Emit source")
        ));

bool tableGen(llvm::raw_ostream &ost, llvm::RecordKeeper &records) {
  switch (action) {
  case PrintRecords:
    ost << records;
    break;
  case DumpJSON:
    EmitJSON(records, ost);
    break;
  case emitNodes:
    emit<NodesEmitter>(records, ost, emitMode);
    break;
  case emitInc:
    emit<IncFileEmitter>(records, ost, emitMode);
    break;
  case emitCommon:
    emit<CommonEmitter>(records, ost, emitMode);
    break;
  case emitTraits:
    emit<TraitsEmitter>(records, ost, emitMode);
    break;
  case emitCodeGen:
    emit<CodeGenEmitter>(records, ost, emitMode);
    break;
  }
  return false;
}

int main(int argc, char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);
  llvm::PrettyStackTraceProgram X(argc, argv);
  fixOptions();
  llvm::cl::HideUnrelatedOptions(TableGenCMDCat);
  llvm::cl::ParseCommandLineOptions(argc, argv);
  llvm::llvm_shutdown_obj Y;
  int val = llvm::TableGenMain(argv[0], &tableGen);
  if (val)
    return val;
  epilogue();
  return 0;
}
