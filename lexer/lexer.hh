#ifndef LEXER_LEXER_HH_
#define LEXER_LEXER_HH_

#include <any>
#include <deque>
#include <iostream>
#include <memory>
#include "toks.hh"

namespace ttc {
using tk = token_kind;
struct token {
  std::string text { };
  token_kind kind { tk::NONE };
  int32_t line { -1 };
  int32_t col { -1 };
  static token create(const char *txt, tk kind, int32_t l, int32_t c) {
    return token { std::string(txt), kind, l, c };
  }
  inline bool valid() const {
    return kind != tk::NONE;
  }
};
inline std::ostream& operator<<(std::ostream& ost, const token& tok) {
  ost << to_string(tok.kind) << "[" << tok.line << ":" << tok.col << "]";
  return ost;
}
class Lexer {
private:
  std::istream &in;
  std::ostream &out;
  std::any _lexer;
  std::deque<token> _tokens { };
  std::deque<token>::iterator _tok { };
  bool _finished { };
  void init();
public:
  Lexer(std::istream &in, std::ostream &out = std::cout) :
      in(in), out(out) {
    init();
  }
  bool valid() const;
  inline auto finished() const {
    return _finished;
  }
  token consume_token();
};
}

#endif
