#include <Driver/Driver.hh>
#include <llvm/Support/CommandLine.h>

namespace {
using namespace llvm;

cl::OptionCategory TlangCMDCat = cl::OptionCategory("tlang");

cl::opt<std::string> OutputFilename(
    "o",
    cl::cat(TlangCMDCat),
    cl::desc("Specify output filename"),
    cl::value_desc("Output file"),
    cl::init("a.ll"));

cl::list<std::string> InputFilename(
    cl::Positional,
    cl::cat(TlangCMDCat),
    cl::desc("Input files"),
    cl::OneOrMore);

cl::opt<bool> Dump(
    "d",
    cl::cat(TlangCMDCat),
    cl::desc("Dump AST"));

cl::opt<bool> NCodeGen(
    "n",
    cl::cat(TlangCMDCat),
    cl::desc("Don't generate LLVM IR code"));
}

namespace tlang::driver {
int Driver::parseCMD(int argc, char **argv) {
  llvm::cl::HideUnrelatedOptions(TlangCMDCat);
  llvm::cl::ParseCommandLineOptions(argc, argv);
  cmdArguments.outputFile = OutputFilename.getValue();
  cmdArguments.inputFiles = InputFilename;
  cmdArguments.dumpAST = Dump;
  cmdArguments.noCodegen = NCodeGen;
  return 0;
}
}
