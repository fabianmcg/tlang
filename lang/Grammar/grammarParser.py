#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Grammar.grammar import Grammar
from Grammar.parseElements import EmptyString, NonTerminal, Production, Rule, Terminal


class GrammarParser:
    def __init__(self, grammar: Grammar) -> None:
        self.grammar = grammar

    def cxx(self):
        grammar = self.grammar
        productions = grammar.productions
        for production in productions.values():
            first = []
