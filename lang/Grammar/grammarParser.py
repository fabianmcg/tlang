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
        self.computeDerivesInEmpty()
        self.computeFirst()

    def computeDerivesInEmpty(self):
        productions = self.grammar.productions
        productionsAsRules = self.grammar.productionsAsRules
        nonTerminals = self.grammar.nonTerminals
        workSet = []

        def check(rule: Rule):
            if rule.data.count == 0:
                rule.data.derivesEmpty = True
                symbol = nonTerminals[rule.data.pid]
                productions[rule.data.pid].data.derivesEmpty = True
                if not symbol.data.derivesEmpty:
                    symbol.data.derivesEmpty = True
                    workSet.append(symbol)

        for rule in productionsAsRules:
            rule.data.count = len(rule)
            check(rule)
        while len(workSet):
            symbol = workSet.pop()
            for x in symbol.data.occurrences:
                rule = productions[x[0]][x[1]]
                rule.data.count -= 1
                check(rule)

    def computeFirst(self):
        grammar = self.grammar
        productions = self.grammar.productions
        visited = set([])

        def visitNonTerminal(identifier: str):
            if identifier in visited:
                return
            visited.add(identifier)
            nonTerminal = grammar.nonTerminal(identifier)
            for rule in grammar.production(identifier):
                if rule.isEmpty():
                    nonTerminal.data.first.add("E")
                    continue
                first = set([])
                for symbol in rule:
                    if isinstance(symbol, Terminal):
                        nonTerminal.data.first.add(symbol.identifier)
                        break
                    visitNonTerminal(symbol.identifier)
                    symbolNT = grammar.nonTerminal(symbol.identifier)
                    first = first | symbolNT.data.first
                    if not symbolNT.data.derivesEmpty:
                        break
                nonTerminal.data.first = nonTerminal.data.first | set([f for f in first if not isinstance(f, str)])
                if rule.data.derivesEmpty:
                    nonTerminal.data.first.add("E")

        for production in productions:
            visitNonTerminal(production)
    
    @staticmethod
    def first(grammar: Grammar, symbolList: list, visitNonTerminal = None):
        first = set([])
        k = 0
        for symbol in symbolList:
            if isinstance(symbol, Terminal):
                first.add(symbol.identifier)
                break
            if visitNonTerminal:
                visitNonTerminal(symbol.identifier)
            symbolNT = grammar.nonTerminal(symbol.identifier)
            first = first | symbolNT.data.first
            if not symbolNT.data.derivesEmpty:
                break
            else:
                k += 1
        if k == len(symbolList):
            first.add("E")
        return first

    def computeFollow(self):
        grammar = self.grammar
        productions = self.grammar.productions
        visited = set([])

        def visitNonTerminal(identifier: str):
            if identifier in visited:
                return
            visited.add(identifier)
            nonTerminal = grammar.nonTerminal(identifier)
            for rule in grammar.production(identifier):
                if rule.isEmpty():
                    nonTerminal.data.first.add("E")
                    continue
                for symbol in rule:
                    if isinstance(symbol, Terminal):
                        nonTerminal.data.first.add(symbol.identifier)
                        break
                    visitNonTerminal(symbol.identifier)
                    symbolNT = grammar.nonTerminal(symbol.identifier)
                    nonTerminal.data.first = nonTerminal.data.first | symbolNT.data.first
                    if not symbolNT.data.derivesEmpty:
                        break
                if rule.data.derivesEmpty:
                    nonTerminal.data.first.add("E")

        for production in productions:
            visitNonTerminal(production)
