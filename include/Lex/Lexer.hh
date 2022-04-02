#ifndef LEX_LEXER_HH
#define LEX_LEXER_HH

#include <any>
#include <iosfwd>
#include <Basic/Exception.hh>
#include <Basic/SourceRange.hh>
#include <Lex/Tokens.hh>

namespace tlang {
namespace lex {
using tk = token_kind;
struct token {
  std::string text { };
  token_kind kind { tk::EOS };
  SourceLocation loc { };
  static token create(const char *txt, tk kind, int32_t l, int32_t c) {
    if (tk::StringLiteral == kind) {
      auto tmp = std::string(txt + 1);
      tmp.erase(--tmp.end());
      return token {tmp , kind, SourceLocation { l, c } };
    }
    return token { std::string(txt), kind, SourceLocation { l, c } };
  }
  inline bool valid() const {
    return kind != tk::EOS;
  }
  inline std::string to_string() const {
    return text + " " + lex::to_string(kind) + loc.to_string();
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
  inline SourceLocation begin() const {
    return getBeginLoc();
  }
  inline SourceLocation end() const {
    return getEndLoc();
  }
  inline SourceRange range() const {
    return getSourceRange();
  }
  inline SourceLocation getBeginLoc() const {
    return loc;
  }
  inline SourceLocation getEndLoc() const {
    return SourceLocation { loc.line, loc.column + static_cast<int>(text.size()) };
  }
  inline SourceRange getSourceRange() const {
    return SourceRange { getBeginLoc(), getEndLoc() };
  }
};
inline std::ostream& operator<<(std::ostream &ost, const token &tok) {
  ost << tok.to_string();
  return ost;
}
struct lexer_exception: public compiler_exception {
  lexer_exception(const token &tok) :
      compiler_exception("Lexer exception: ") {
    message += tok.to_string();
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
