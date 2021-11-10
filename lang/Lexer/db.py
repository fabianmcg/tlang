#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Lexer.token import Definition


class LexerDB:
    def __init__(self):
        self.definitions = DotDict()
        self.tokens = DotDict()

    def __getattr__(self, attr):
        return self.tokens.__getattr__(attr)

    def __str__(self) -> str:
        return "Definitions:\n\t{}\nTokens:\n\t{}".format(
            "\n\t".join(list(map(str, self.definitions.values()))), "\n\t".join(list(map(str, self.tokens.values())))
        )

    __repr__ = __str__

    def addToken(self, kind, identifier, *rules):
        self.tokens[identifier] = kind(identifier, *rules)

    def addDefinition(self, identifier, *rules):
        self.definitions[identifier] = Definition(identifier, *rules)

    def getTokens(self, function=lambda x: x):
        return DotDictWrapper(self.tokens, function)
