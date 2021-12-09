#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Conf.toks import makeLexer
from AST.confParser import makeAST
from Grammar.confParser import makeParser


def main():
    tokens = makeLexer()
    grammar = makeParser("Conf/grammar.conf", tokens)
    print(grammar)
    # tokens.generateLexer(".", "Templates")
    # ast = makeAST("Conf/")
    # ast.generateASTNodes("./", "./", "Templates/header.hh.j2")
    # ast.generateRecursiveASTVisitor("./", "Templates")


if __name__ == "__main__":
    main()
