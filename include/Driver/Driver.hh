#ifndef DRIVER_DRIVER_HH
#define DRIVER_DRIVER_HH

#include <filesystem>
#include <Frontend/CompilerInvocation.hh>

namespace tlang::driver {
class Driver {
private:
  struct Arguments {
    std::vector<std::string> inputFiles { };
    std::string outputFile { };
    bool dumpAST { };
    bool dumpSymbols { };
    bool noCodegen { };
  };
  Arguments cmdArguments { };
  CompilerInvocation compilerContext { };
public:
  int run(int argc, char **argv);
private:
  int parseCMD(int argc, char **argv);
  int parseFiles();
  int semaAnalysis(CompilerInvocation &context);
  int runPasses(CompilerInvocation &context);
  int codeGen(CompilerInvocation &context, const std::filesystem::path &file);
  void dump();
};
}
#endif
