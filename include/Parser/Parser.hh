#ifndef PARSER_PARSER_HH
#define PARSER_PARSER_HH

#include <Lex/Lexer.hh>
#include <list>
#include <AST/ASTContext.hh>

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
  ModuleDecl* parse(ASTContext &ctx, const std::string &module_name);
protected:
  using tok_it_t = std::list<token>::iterator;
  using context_t = tok_it_t;
  const token& peekToken() const;
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
