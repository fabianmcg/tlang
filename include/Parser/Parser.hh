#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <list>
#include "AST/Include.hh"
#include "Lexer/Lexer.hh"

namespace tlang::parser {
using namespace lex;
class Parser {
public:
  friend class ParserHelper;
  Parser(lex::Lexer &lexer) :
      lexer(lexer) {
    current_token = tokens.end();
    consumeToken();
  }
  ASTContext parse();
protected:
  void ParseASTContext(ASTContext &ctx);
  using tok_it_t = std::list<token>::iterator;
  using context_t = tok_it_t;
  token peekToken();
  void consumeToken();
  void restoreContext(tok_it_t);
  tok_it_t getContext();
  Lexer &lexer;
  std::list<token> tokens { };
  tok_it_t current_token;
  token tok { };
};
}
#endif
