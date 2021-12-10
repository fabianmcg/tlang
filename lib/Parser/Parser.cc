#include "Parser/Parser.hh"

namespace tlang::parser {
ASTContext Parser::parse() {
  ASTContext context;
  ParseASTContext(context);
  return context;
}
void Parser::consumeToken() {
  auto tmp = current_token;
  if (tmp != tokens.end() && (++tmp) != tokens.end()) {
    ++current_token;
    tok = *current_token;
  } else {
    tok = lexer.consume_token();
    current_token = tokens.insert(tokens.end(), tok);
  }
}
token Parser::peekToken() {
  auto tmp = current_token;
  if (tmp != tokens.end() && (++tmp) != tokens.end()) {
    return *tmp;
  } else {
    auto tmp = lexer.consume_token();
    tokens.push_back(tmp);
    return tmp;
  }
}
Parser::tok_it_t Parser::getContext() {
  return current_token;
}
void Parser::restoreContext(tok_it_t it) {
  current_token = it;
  tok = *it;
}
}
