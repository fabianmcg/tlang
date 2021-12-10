#ifndef __LEXER_HH__
#define __LEXER_HH__

#include <any>
#include <deque>
#include <iostream>
#include <Common/Macros.hh>
#include <Common/Exception.hh>
#include <Common/SourceRange.hh>
#include <Tokens.hh>

namespace __lang_np__ {
namespace __lex_np__ {
using tk = token_kind;
struct token {
  std::string text { };
  token_kind kind { tk::NONE };
  SourceLocation loc { };
  static token create(const char *txt, tk kind, int32_t l, int32_t c) {
    return token { std::string(txt), kind, SourceLocation { l, c } };
  }
  inline bool valid() const {
    return kind != tk::NONE;
  }
  inline std::string to_tring() const {
    return text + " " + to_string(kind) + loc.to_string();
  }
  inline bool is(token_kind k) const {
    return kind == k;
  }
  inline bool isNot(token_kind k) const {
    return kind != k;
  }
  inline std::string value() const {
    return text;
  }
};
inline std::ostream& operator<<(std::ostream &ost, const token &tok) {
  ost << tok.to_tring();
  return ost;
}
struct lexer_exception: public compiler_exception {
  lexer_exception(const token &tok) :
      compiler_exception("Lexer exception: ") {
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
} // namespace __lex_np__
} // namespace __lang_np__
#endif
