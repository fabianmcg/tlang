#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Conf.toks import makeLexerDB
from Lang.db import LangDB
from Conf.nodes import langNodes
from Conf.grammar import grammar
from Lexer.generate import generateLexer
from Grammar.generate import generateParser
from Lang.generate import generateAstNodes


def main():
    tokens = makeLexerDB()
    lang = LangDB(tokens)
    langNodes(lang)
    grammar(lang)
    generateAstNodes(lang, "./")
    generateLexer(lang.tokens, "./")
    # print(lang)
    # generateParser(lang.grammar, "./")


if __name__ == "__main__":
    main()
