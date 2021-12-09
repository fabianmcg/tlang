#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lexer.lexer import Lexer
from Utility.dotDict import DotDict


class Grammar:
    def __init__(self, lexer: Lexer):
        self.tokens = lexer.tokens
        self.terminals = {tok.parseRepr(): tok for tok in self.tokens.values()}
        self.productions = []

    def __str__(self) -> str:
        return "{}".format("\n".join(list(map(str, self.productions))))

    def __repr__(self) -> str:
        return str(self)
