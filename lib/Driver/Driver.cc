#include <Common/Io.hh>
#include <Lexer/Lexer.hh>
#include <Parser/Parser.hh>
#include <Sema/Sema.hh>
#include <Io/ASTIo.hh>

int main(int argc, char **argv) {
  using namespace _lnp_;
  using namespace _pnp_;
  using namespace _snp_;
  using namespace _astnp_;
  auto fs = unique_fstream::open_istream("main.tt");
  Lexer lex(*fs);
  Parser parser = Parser(lex);
  ASTContext context = parser.parse();
//  dump(*context);
  Sema sema(context);
  sema.analyze();
  dump(*context);
  return 0;
}
