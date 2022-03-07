#include <Common/Io.hh>
#include <Lexer/Lexer.hh>
#include <Parser/Parser.hh>
#include <Sema/Sema.hh>
#include <CodeGen/CodeGen.hh>
#include <Io/ASTIo.hh>

int main(int argc, char **argv) {
  using namespace _lnp_;
  using namespace _pnp_;
  using namespace _snp_;
  using namespace _astnp_;
  auto fs = unique_fstream::open_istream("main.tt");
  Lexer lex(*fs);
  Parser parser(lex);
  ASTContext context;
  parser.parse(context, "main");
  Sema sema(context);
  sema.analyze();
  dump(*context);
  context.print_symbols(std::cerr);
  codegen::CodeGen gen(context, "main");
  auto os = unique_fstream::open_ostream("main.ll");
  gen.emit(*context, *os);
  std::cerr << "**" << std::endl;
  return 0;
}
