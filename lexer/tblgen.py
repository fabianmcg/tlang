#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Fri 29 08:35:00 2021

@author: fabian
"""

import argparse
import yaml
import pathlib

namespace = "ttc"
definitions = """%option c++
%option noyywrap
%option yylineno
%option yyclass="CXXLexer"

%top{
#include <toks.hh>
#include <lexer.hh>
#include <iostream>
#include <memory>
}

%{
#define YY_USER_ACTION          \\
  __yycolumn  = __yybufpos;     \\
  __yybufpos += yyleng;
class CXXLexer : public yyFlexLexer {
public:
  using yyFlexLexer::yyFlexLexer;
  friend void comment(CXXLexer &lex);
  virtual int yylex();
  inline int colno() const {
    return __yycolumn;
  }
private:
  int __yycolumn = 1;
  int __yybufpos = 1;
};
inline CXXLexer* getLex(std::any lex) {
  return std::any_cast<std::shared_ptr<CXXLexer>>(lex).get();
}
void comment(CXXLexer &lex);
%}
"""

lexRules = """
{: <30}{}
{: <30}{{}}
""".format(
    '"/*"', "{ comment(*this); }", '"//".*'
)

code = """
namespace ttc {
bool Lexer::valid() const {
  return _lexer.has_value() && getLex(_lexer) && !_finished;
}
void Lexer::init() {
  this->_lexer = std::make_shared<CXXLexer>(this->in, this->out);
}
token Lexer::consume_token() {
  auto lex = getLex(_lexer);
  int tok = lex->yylex();
  return token::create( lex->YYText(), static_cast<ttc::tk>(tok), lex->lineno(), lex->colno());
}
}
void comment(CXXLexer &lex) {
  int c;
  while ((c = lex.yyinput()) != 0) {
    lex.__yybufpos = c == '\\n' ? 1 : lex.__yybufpos + 1;
    if (c == '*') {
      while (++lex.__yybufpos, ((c = lex.yyinput()) == '*'));
      if (c == '/') return;
      if (c == 0)
        throw(ttc::lexer_exception(ttc::token::create( lex.YYText(), ttc::tk::UNKW, lex.lineno(), lex.colno())));
    }
  }
}
"""


def parseArgs(argsString=""):
    parser = argparse.ArgumentParser(
        description="tblgen",
        add_help=True,
        formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=60),
    )
    parser.add_argument(
        "-i",
        metavar="<input file name>",
        type=str,
        default="toks.yml",
        help="input file name",
    )
    parser.add_argument(
        "-o",
        metavar="<output dir>",
        type=str,
        default="./",
        help="output dir name",
    )
    if len(argsString) == 0:
        args = parser.parse_args()
    else:
        args = parser.parse_args(argsString.split(" "))
    return args


def genEnum(r, v):
    if "value" in v:
        r = "{} = '{}'".format(r, v["value"])
    if isinstance(v["rule"], list):
        v = " | ".join(v["rule"])
    else:
        v = v["rule"]
    return r, v


def genTokens(rules, dir):
    toks = ""
    switch = "  switch (kind) {\n"
    switch += '  case tk::UNKW:\n    return "unknown";\n'
    switch += '  case tk::NONE:\n    return "none";\n'
    for k, v in rules.items():
        kk, vv = genEnum(k, v)
        toks += "  {: <30}//  {}\n".format(kk + ",", vv)
        switch += '  case tk::{}:\n    return "{}";\n'.format(k, k if "value" not in v else v["value"])
    switch += '  default:\n    return "";\n  }\n'
    with open(pathlib.Path(dir, "toks.hh"), "w") as file:
        print("#ifndef LEXER_TOKENS_HH_\n#define LEXER_TOKENS_HH_\n", file=file)
        print("#include <cstdint>\n#include <iostream>\n#include <string>\n", file=file)
        print(
            "namespace {} {{\nenum class token_kind {{\n  UNKW = -1,\n  NONE = 0,\n  FIRST = 1024,\n{}}};".format(
                namespace, toks
            ),
            file=file,
        )
        print("using tk = token_kind;", file=file)
        print("inline std::string to_string(tk kind) {{\n{}}}".format(switch), file=file)
        print(
            "inline std::ostream& operator<<(std::ostream& ost, token_kind kind) {\n  ost << to_string(kind);\n  return ost;\n}",
            file=file,
        )
        print("}\n#endif", file=file)


def genLex(rules, dir):
    global definitions, lexRules
    for k, v in rules["defs"].items():
        definitions += "\n{: <20} {}".format(k, v)
    definitions += "\n"
    for k, v in rules["rules"].items():
        if isinstance(v["rule"], list):
            for r in v["rule"]:
                lexRules += "{: <30}{{ return static_cast<int>(ttc::tk::{}); }}\n".format(r, k)
        else:
            lexRules += "{: <30}{{ return static_cast<int>(ttc::tk::{}); }}\n".format('"' + v["rule"] + '"', k)
    lexRules += "{: <30}{{}}\n{: <30}{{ {}; }}\n".format(
        "{WS}+", ".", "throw(ttc::lexer_exception(ttc::token::create( YYText(), ttc::tk::UNKW, lineno(), colno()) ));"
    )
    lexRules += "{: <30}{{{}}}\n".format("\\n", " __yybufpos = 1; ")
    with open(pathlib.Path(dir, "lex.yy"), "w") as file:
        print("{}\n%%{}\n%%{}".format(definitions, lexRules, code), file=file)


def main():
    args = parseArgs()
    with open(args.i, "r") as file:
        rules = yaml.safe_load(file)
        genTokens(rules["rules"], args.o)
        genLex(rules, args.o)


if __name__ == "__main__":
    main()
