#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Grammar.grammar import Grammar
from Grammar.parseElements import EmptyString, NonTerminal


class GrammarParser:
    def __init__(self, grammar: Grammar) -> None:
        self.grammar = grammar
        self.first = {}
        self.follow = {}
        self.derivesEmpty = set([])

    def computeDerivesInEmpty(self):
        productions = self.grammar.productions
        empty = self.derivesEmpty
        productionStack = ["__top__"]
        visited = set([])
        parent = {}
        while len(productionStack):
            pid = productionStack.pop()
            if pid not in productions or pid in visited:
                continue
            visited.add(pid)
            production = productions[pid]
            children = [rule[0] for rule in production if isinstance(rule[0], NonTerminal)]
            for symbol in children:
                if isinstance(symbol, EmptyString):
                    empty.add(pid)
                parent[symbol] = pid


    def computeFirst(self):
        grammar = self.grammar
        for terminal in grammar.terminals:
            self.first[terminal] = set([terminal])
        for production in grammar.productions:
            first = set([])
            for rule in production:
                if rule.isEmpty():
                    first.add(EmptyString())
                else:
                    for symbol in rule:
                        pass
            self.first[production.asNonTerminal()] = first
