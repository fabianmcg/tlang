#include <io.hh>
#include <macros.hh>
#include <lexer.hh>

int main(int argc, char **argv) {
  using namespace _lnp_;
  auto fs = unique_fstream::open_istream("main.tt");
  Lexer lex(*fs);
  while(lex.valid()) {
    auto tok = lex.consume_token();
    std::cerr << tok << std::endl;
    if (!tok.valid())
      break;
  }
  return 0;
}
