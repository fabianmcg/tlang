#ifndef __DRIVER_HH__
#define __DRIVER_HH__

#include <filesystem>
#include <AST/ASTContext.hh>

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
  ASTContext context { };
public:
  int run(int argc, char **argv);
private:
  int parseCMD(int argc, char **argv);
  int parseFiles();
  int parseFile(ASTContext &context, const std::filesystem::path &file);
  int semaAnalysis(ASTContext &context);
  int codeGen(ASTContext &context, const std::filesystem::path &file);
  void dump();
};
}
#endif
