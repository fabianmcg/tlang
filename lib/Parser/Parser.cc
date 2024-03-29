#include <Parser/Parser.hh>

namespace tlang::parser {
void Parser::consumeToken() {
//  auto tmp = current_token;
//  if (tmp != tokens.end() && (++tmp) != tokens.end()) {
//    ++current_token;
//    tok = *current_token;
//  } else {
//    tok = lexer.consume_token();
//    current_token = tokens.insert(tokens.end(), tok);
//  }
  tok = lexer.consume_token();
}
const token& Parser::peekToken() const {
  return tok;
}
Parser::tok_it_t Parser::getContext() {
  return current_token;
}
void Parser::restoreContext(tok_it_t it) {
  current_token = it;
  tok = *it;
}
}
