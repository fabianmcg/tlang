#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <list>
#include "AST/Include.hh"
#include "Lexer/Lexer.hh"

namespace tlang::parser {
enum class ParseResult {
  success,
  fail,
  empty
};
template <typename T>
struct ParseReturnType {
  ParseResult parse_result { };
  T data { };
  ParseReturnType() = default;
  ~ParseReturnType() = default;
  ParseReturnType(ParseReturnType&&) = default;
  ParseReturnType(const ParseReturnType&) = default;
  ParseReturnType& operator=(ParseReturnType&&) = default;
  ParseReturnType& operator=(const ParseReturnType&) = default;
  operator bool() const {
    return parse_result == ParseResult::success;
  }
  bool operator !() const {
    return parse_result != ParseResult::fail;
  }
  T& operator*() {
    return data;
  }
  static ParseReturnType empty() {
    return ParseReturnType { ParseResult::empty };
  }
  static ParseReturnType fail() {
    return ParseReturnType { ParseResult::fail };
  }
};
template <typename T>
using parse_return_t = ParseReturnType<T>;
using namespace lex;
template <typename T>
using unique = std::unique_ptr<T>;
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
  template <typename T, typename ...Args>
  unique<T> make_unique(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }
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
