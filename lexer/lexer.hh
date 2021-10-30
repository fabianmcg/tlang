#ifndef LEXER_LEXER_HH_
#define LEXER_LEXER_HH_

#include <any>
#include <deque>
#include <iostream>
#include <memory>
#include "toks.hh"
#include "exception.hh"
#include "extent.hh"

namespace ttc {
using tk = token_kind;
struct token {
  std::string text { };
  token_kind kind { tk::NONE };
  location loc { };
  static token create(const char *txt, tk kind, int32_t l, int32_t c) {
    return token { std::string(txt), kind, location { l, c } };
  }
  inline bool valid() const {
    return kind != tk::NONE;
  }
  inline std::string to_tring() const {
    return to_string(kind) + loc.to_string();
  }
};
inline std::ostream& operator<<(std::ostream &ost, const token &tok) {
  ost << tok.to_tring();
  return ost;
}
struct lexer_exception: public compiler_exception {
  lexer_exception(const token &tok) : compiler_exception("Lexer exception: ") {
    message += tok.to_tring();
  }
};
class Lexer {
private:
  std::istream &in;
  std::ostream &out;
  std::any _lexer;
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
