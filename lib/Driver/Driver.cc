#include <Driver/Driver.hh>
#include <Driver/Arguments.hh>
#include <Support/UniqueStream.hh>
#include <Lex/Lexer.hh>
#include <Parser/Parser.hh>
#include <Sema/Sema.hh>
//#include <Analysis/Analysis.hh>
//#include <CodeGen/CodeGen.hh>
#include <Io/ASTIo.hh>

namespace tlang::driver {

int Driver::run(int argc, char **argv) {
  int stage { 0 };
  if (++stage && parseCMD(argc, argv))
    return stage;
  if (++stage && parseFiles())
    return stage;
  if (++stage && semaAnalysis(context))
    return stage;
//  if (++stage && codeAnalysis(context))
//    return stage;
  dump();
//  if (++stage && codeGen(context, std::filesystem::path { cmdArguments.outputFile }))
//    return stage;
  return 0;
}
int Driver::parseCMD(int argc, char **argv) {
  arguments args("Tlang compiler", 120, true);
  args.add_opt("inputFiles,I", cmdArguments.inputFiles, "Input files");
  args.add_popt("inputFiles", -1);
  args.add_opt("outputFile,o", cmdArguments.outputFile, "a.ll", "Output file");
  args.add_opt("dumpAST,d", args.flag_opt(cmdArguments.dumpAST), "Dump AST");
  args.add_opt("dumpSymbols,S", args.flag_opt(cmdArguments.dumpSymbols), "Dump Symbol Table");
  args.add_opt("noCodeGen,n", args.flag_opt(cmdArguments.noCodegen), "Don't generate LLVM IR code");
  return args.parse(argc, argv, true);
}
int Driver::parseFiles() {
  for (auto &file : cmdArguments.inputFiles)
    if (parseFile(context, std::filesystem::path { file }))
      return 1;
  return cmdArguments.inputFiles.empty() ? 1 : 0;
}
int Driver::parseFile(ASTContext &context, const std::filesystem::path &file) {
  if (std::filesystem::exists(file)) {
    std::cerr << "Parsing file: " << file << std::endl;
    auto fs = unique_fstream::open_istream(file.string());
    lex::Lexer lex(*fs);
    parser::Parser parser(lex);
    parser.parse(context, "main");
    std::cerr << "Done parsing: " << file << std::endl;
    return 0;
  }
  return 1;
}
int Driver::semaAnalysis(ASTContext &context) {
  Sema sema(context);
  sema.run();
  return 0;
}
int Driver::codeAnalysis(ASTContext &context) {
//  CodeAnalysis pass(context);
//  pass.run();
  return 0;
}
int Driver::codeGen(ASTContext &context, const std::filesystem::path &file) {
  if (cmdArguments.noCodegen)
    return 0;
//  if (!file.empty()) {
//    std::cerr << "Generating: " << file << std::endl;
//    codegen::CodeGen emitter(context, "main");
//    auto os = unique_fstream::open_ostream(file.string());
//    emitter.emit(*context, *os);
//    std::cerr << "Finished generating: " << file << std::endl;
//    return 0;
//  }
  return 1;
}
void Driver::dump() {
  if (cmdArguments.dumpAST)
    tlang::dump(*context);
}
}
