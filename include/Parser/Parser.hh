#ifndef __PARSER_HH__
#define __PARSER_HH__

#include <list>
#include "AST/Include.hh"
#include "Lexer/Lexer.hh"

namespace tlang::parser {
template <typename T>
using unique = std::unique_ptr<T>;
template <typename T, typename ...Args>
unique<T> make_unique_ptr(Args &&...args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}
enum class ParseResult {
  success,
  fail,
  empty
};
template <typename T>
struct ParseReturnType {
  unique<T> data { };
  ParseResult parse_result { };
  ParseReturnType(ParseResult result) :
      parse_result(result) {
  }
  ParseReturnType(unique<T> &&data, ParseResult result) :
      data(std::move(data)), parse_result(result) {
  }
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
    return parse_result == ParseResult::fail;
  }
  unique<T>& operator*() {
    return data;
  }
  ParseReturnType& operator++() {
    parse_result = ParseResult::success;
    return *this;
  }
  template <typename V>
  ParseReturnType& operator=(ParseReturnType<V> &&result) {
    parse_result = result.parse_result;
    if (result.parse_result == ParseResult::success)
      data = std::move(result.data);
    return *this;
  }
  template <typename ...Args>
  static ParseReturnType make(Args &&...args) {
    return ParseReturnType(make_unique_ptr<T>(std::forward<Args>(args)...), ParseResult::success );
  }
  static ParseReturnType empty() {
    return ParseReturnType { nullptr, ParseResult::empty };
  }
  static ParseReturnType fail() {
    return ParseReturnType { nullptr, ParseResult::fail };
  }
};
template <typename T>
using parse_return_t = ParseReturnType<T>;
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
