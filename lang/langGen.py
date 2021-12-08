#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

# from Conf.toks import makeLexerDB
# from Lang.db import LangDB
# from Conf.nodes import langNodes
# from Lexer.generate import generateLexer
# from Lang.generate import generateAstNodes

# from Grammar.parser import parse
from Cxx.struct import ParentList
from AST.confParser import parse

# from Lang.nodes import CodeSection, HeaderSection
# from Lang.AST import var


def main():
    # tokens = makeLexerDB()
    # lang = LangDB(tokens)
    # langNodes(lang)
    # generateAstNodes(lang, "./gen")
    # generateLexer(lang.tokens, "./gen")
    # grammar(lang)
    # generateParser(lang.grammar, "./")
    parse("Conf/nodes.conf")


if __name__ == "__main__":
    main()
