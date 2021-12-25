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

    def predictSet(self, rule: Rule, production: Production):
        predict = rule.firstSet.copy()
        if "E" in predict:
            predict.remove("E")
            predict = predict | production.nonTerminal.followSet
        return predict

    def cxxRule(self):
        pass

    def cxxProductionLL1(self, production: Production):
        src = "switch (tok.kind) {{{} default: break;}}"
        tmp = ""
        for rule in production:
            predict = self.predictSet(rule, production)
            
        return src

    def cxxProduction(self, production: Production):
        src = ""
        if production.isLL1:
            return self.cxxProductionLL1(production)
        return src

    def cxx(self):
        grammar = self.grammar
        productions = grammar.productions
        # p = [self.cxxProduction(production) for production in productions.values()]
        p = []
        for production in productions.values():
            print(production.identifier, production.nonTerminal.firstSet, production.nonTerminal.followSet)
        return "\n".join(p)
