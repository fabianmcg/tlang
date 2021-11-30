#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Conf.toks import makeLexerDB
from Lang.db import LangDB
from Conf.nodes import langNodes
from Lexer.generate import generateLexer
from Lang.generate import generateAstNodes


def main():
    tokens = makeLexerDB()
    lang = LangDB(tokens)
    langNodes(lang)
    generateAstNodes(lang, "./gen")
    generateLexer(lang.tokens, "./gen")
    # grammar(lang)
    # generateParser(lang.grammar, "./")


if __name__ == "__main__":
    main()
