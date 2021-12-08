#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <list>

#include "ast_nodes.hh"
#include "ast_context.hh"
#include "lexer.hh"
namespace tlang::parser {
using namespace lex;
class Parser {
public:
  Parser(lex::Lexer &lexer) :
      lexer(lexer) {
    current_token = tokens.end();
    consumeToken();
  }
  ASTContext parse();
  void handleModule(ASTContext &context);
  inline bool isValid() const {
    return tok.valid();
  }
protected:
  template <typename T>
  using unique_t = std::unique_ptr<T>;
  unique_t<FunctionDecl> handleFunction();
  void handleParameters(FunctionDecl &function);
  unique_t<VarDecl> handleVar();
  unique_t<ParDecl> handlePar();
  unique_t<QualType> handleType();
  template <typename T, typename ...Args>
  unique_t<T> make_unique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
  using tok_it_t = std::list<token>::iterator;
  token peekToken();
  void consumeToken();
  void acceptRule(tok_it_t);
  void restoreCtx(tok_it_t);
  Lexer &lexer;
  std::list<token> tokens { };
  tok_it_t current_token;
  token tok { };
};
}
#endif
