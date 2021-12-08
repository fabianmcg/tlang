#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Conf.toks import makeLexer
from AST.confParser import makeAST


def main():
    tokens = makeLexer()
    ast = makeAST("Conf/")
    # ast.generateASTNodes("./", "./", "Templates/header.hh.j2")
    ast.generateRecursiveASTVisitor("./", "Templates")
    # tokens.generateLexer(".", "Templates")
    # lang = LangDB(tokens)
    # langNodes(lang)
    # generateAstNodes(lang, "./gen")
    # generateLexer(lang.tokens, "./gen")
    # grammar(lang)
    # generateParser(lang.grammar, "./")
    # db = parse("Conf/nodes_ax.conf")
    # print(db.cxx())


if __name__ == "__main__":
    main()
