#include <Driver/Driver.hh>
#include <Driver/Arguments.hh>
#include <Support/UniqueStream.hh>
#include <Sema/Sema.hh>
#include <CodeGen/CodeGen.hh>
#include <Io/ASTIo.hh>
#include <llvm/Support/raw_ostream.h>
#include <Passes/Pipelines.hh>

namespace tlang::driver {

int Driver::run(int argc, char **argv) {
  int stage { 0 };
  if (++stage && parseCMD(argc, argv))
    return stage;
  if (++stage && parseFiles())
    return stage;
  if (++stage && semaAnalysis(compilerContext))
    return stage;
//  dump();
  if (++stage && runPasses(compilerContext))
    return stage;
  dump();
  if (++stage && codeGen(compilerContext, std::filesystem::path { cmdArguments.outputFile }))
    return stage;
  return 0;
}
int Driver::parseCMD(int argc, char **argv) {
  arguments args("Tlang compiler", 120, true);
  args.add_opt("inputFiles,I", cmdArguments.inputFiles, "Input files");
  args.add_popt("inputFiles", -1);
  args.add_opt("outputFile,o", cmdArguments.outputFile, "a", "Output file");
  args.add_opt("dumpAST,d", args.flag_opt(cmdArguments.dumpAST), "Dump AST");
  args.add_opt("dumpSymbols,S", args.flag_opt(cmdArguments.dumpSymbols), "Dump Symbol Table");
  args.add_opt("noCodeGen,n", args.flag_opt(cmdArguments.noCodegen), "Don't generate LLVM IR code");
  return args.parse(argc, argv, true);
}
int Driver::semaAnalysis(CompilerInvocation &CI) {
  Sema sema(CI);
  sema.run();
  return 0;
}
int Driver::runPasses(CompilerInvocation &CI) {
  return MainPipeline::run(CI);
}
int Driver::codeGen(CompilerInvocation &CI, const std::filesystem::path &file) {
  if (cmdArguments.noCodegen)
    return 0;
  if (!file.empty()) {
    codegen::CodeGen emitter(CI);
    return emitter.emit(file);
  }
  return 1;
}
void Driver::dump() {
  if (cmdArguments.dumpAST)
    tlang::dump(**compilerContext);
}
}
