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
  if (++stage && runPasses(compilerContext))
    return stage;
  dump();
  if (++stage && codeGen(compilerContext, std::filesystem::path { cmdArguments.outputFile }))
    return stage;
  return 0;
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
