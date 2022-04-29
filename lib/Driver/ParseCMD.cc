#include <Driver/Driver.hh>
//#include <Driver/Arguments.hh>
#include <llvm/Support/CommandLine.h>

namespace {
using namespace llvm;
cl::OptionCategory TlangCMDCat = cl::OptionCategory("tlang");
cl::opt<std::string> OutputFilename("o", cl::cat(TlangCMDCat), cl::desc("Specify output filename"), cl::value_desc("<output file>"),
    cl::init("a.ll"));
cl::list<std::string> InputFilename(cl::Positional, cl::cat(TlangCMDCat), cl::desc("<Input files>"), cl::OneOrMore);
}
namespace tlang::driver {
int Driver::parseCMD(int argc, char **argv) {
  llvm::cl::HideUnrelatedOptions(TlangCMDCat);
  llvm::cl::ParseCommandLineOptions(argc, argv);
  cmdArguments.outputFile = OutputFilename.getValue();
  cmdArguments.inputFiles = InputFilename;
//  arguments args("Tlang compiler", 120, true);
//  args.add_opt("inputFiles,I", cmdArguments.inputFiles, "Input files");
//  args.add_popt("inputFiles", -1);
//  args.add_opt("outputFile,o", cmdArguments.outputFile, "a", "Output file");
//  args.add_opt("dumpAST,d", args.flag_opt(cmdArguments.dumpAST), "Dump AST");
//  args.add_opt("dumpSymbols,S", args.flag_opt(cmdArguments.dumpSymbols), "Dump Symbol Table");
//  args.add_opt("noCodeGen,n", args.flag_opt(cmdArguments.noCodegen), "Don't generate LLVM IR code");
//  return args.parse(argc, argv, true);
  return 0;
}
}
