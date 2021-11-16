#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lexer.db import LexerDB
from Utility.dotDict import DotDict
from Grammar.rule import Terminal, NonTerminal, Match, Rule


class GrammarDB:
    def __init__(self, tokens: LexerDB):
        self.terminals = tokens.getTokens(lambda x: Match(Terminal(x)))
        self.nonTerminals = DotDict()
        self.rules = DotDict()

    def __str__(self) -> str:
        return "Grammar:\n{}".format("\n".join(list(map(str, self.rules.values()))))

    __repr__ = __str__

    def addRule(self, identifier, isNode=True, returnType=None):
        self.rules[identifier] = Rule(identifier, isNode, returnType)
        self.nonTerminals[identifier] = Match(NonTerminal(identifier))
        return self.rules[identifier]
