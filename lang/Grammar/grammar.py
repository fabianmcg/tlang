#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from numpy.core.fromnumeric import prod
from Grammar.parseElements import EmptyString, NonTerminal, Production, Rule, Terminal
from Lexer.lexer import Lexer
from Utility.util import getShortRepr


class Grammar:
    def __init__(self, lexer: Lexer):
        self.tokens = {tok.parseRepr(): tok for tok in lexer.tokens.values()}
        self.terminals = {}
        self.nonTerminals = {}
        self.productions = {}
        self.productionsAsRules = {}

    def __str__(self) -> str:
        return "{}".format("\n".join(list(map(str, self.productions))))

    def __repr__(self) -> str:
        return str(self)

    def terminal(self, key: str) -> Terminal:
        return self.terminals[key]

    def nonTerminal(self, key: str) -> NonTerminal:
        return self.nonTerminals[key]

    def production(self, key: str) -> Production:
        return self.productions[key]

    def setProductions(self, productions: list):
        self.productions = {production.identifier: production for production in productions}
        self.consolidate()

    def shortRepr(self) -> str:
        return "{}".format("\n".join(list(map(getShortRepr, self.productions.values()))))

    def traverseDFS(self, fn, stack: list = None):
        productions = self.productions
        if stack == None:
            stack = ["__top__"]
        stack = [(production, None) for production in stack]
        visitStatus = {}

        def getStatus(nodeId):
            if nodeId not in visitStatus:
                visitStatus[nodeId] = [0]
            return visitStatus[nodeId]

        while len(stack):
            pid = stack.pop()
            if pid[0] not in productions:
                continue
            status = getStatus(pid[0])
            if status[0] == 0:
                production = productions[pid[0]]
                children = [
                    symbol.identifier
                    for rule in production
                    for symbol in rule
                    if isinstance(symbol, NonTerminal) and not isinstance(symbol, EmptyString)
                ]
                stack.append((pid[0], pid[1]))
                for nonterminal in reversed(children):
                    if nonterminal in productions and getStatus(nonterminal)[0] == 0:
                        stack.append((nonterminal, pid[0]))
                status[0] += 1
            elif status[0] == 1:
                fn(productions[pid[0]], pid[1])
                status[0] += 1

    def consolidate(self):
        productions = self.productions
        self.nonTerminals = {p.identifier: p.nonTerminal for p in productions.values()}
        terminals = self.terminals
        nonTerminals = self.nonTerminals
        for production in productions.values():
            for ruleId, rule in enumerate(production):
                for nodeId, node in enumerate(rule):
                    symbol = None
                    if isinstance(node, Terminal):
                        if node.identifier not in terminals:
                            symbol = node.clone()
                            terminals[node.identifier] = symbol
                        else:
                            symbol = terminals[node.identifier]
                    elif isinstance(node, NonTerminal):
                        if node.identifier not in nonTerminals:
                            raise Exception("Undefined nonterminal: {}".format(node))
                        else:
                            symbol = nonTerminals[node.identifier]
                    else:
                        raise (Exception("Invalid node"))
                    symbol.occurrencesSet.append((production.identifier, ruleId, nodeId))
        self.productionsAsRules = [rule for production in productions.values() for rule in production]
        self.derivesEmpty()
        self.computeFirstSet()
        self.computeFollow()

    def derivesEmpty(self):
        productions = self.productions
        productionsAsRules = self.productionsAsRules
        workSet = []

        def check(rule: Rule):
            if rule.countEmpty == 0:
                rule.derivesEmpty = True
                symbol = self.nonTerminal(rule.productionId)
                if not symbol.derivesEmpty:
                    symbol.derivesEmpty = True
                    workSet.append(symbol)

        for rule in productionsAsRules:
            rule.countEmpty = len(rule)
            check(rule)
        while len(workSet):
            symbol = workSet.pop()
            for x in symbol.occurrencesSet:
                rule = productions[x[0]][x[1]]
                rule.countEmpty -= 1
                check(rule)

    def firstSet(self, symbolList: list, visitNonTerminal=None):
        first = set([])
        k = 0
        for symbol in symbolList:
            if isinstance(symbol, Terminal):
                first.add(symbol.identifier)
                break
            if visitNonTerminal:
                visitNonTerminal(symbol.identifier)
            symbolNT = self.nonTerminal(symbol.identifier)
            first = first | symbolNT.firstSet
            if not symbolNT.derivesEmpty:
                break
            else:
                k += 1
        if "E" in first:
            first.remove("E")
        if k == len(symbolList) and k > 0:
            first.add("E")
        return first

    def computeFirstSet(self):
        productions = self.productions
        visited = set([])

        def visitNonTerminal(identifier: str):
            if identifier in visited:
                return
            visited.add(identifier)
            nonTerminal = self.nonTerminal(identifier)
            for rule in self.production(identifier):
                if rule.isEmpty():
                    nonTerminal.firstSet.add("E")
                    continue
                first = self.firstSet(rule.rule, visitNonTerminal)
                rule.firstSet = first
                nonTerminal.firstSet = nonTerminal.firstSet | first
                if rule.derivesEmpty:
                    nonTerminal.firstSet.add("E")

        for production in productions:
            visitNonTerminal(production)

    def computeFollow(self):
        nonTerminals = self.nonTerminals
        visited = set([])

        def visitNonTerminal(nonTerminal: NonTerminal):
            if nonTerminal.identifier in visited:
                return
            visited.add(nonTerminal.identifier)
            for occurrence in nonTerminal.occurrencesSet:
                rule = self.production(occurrence[0])[occurrence[1]]
                rule = rule[occurrence[2] + 1 :]
                first = self.firstSet(rule)
                if len(first):
                    nonTerminal.followSet = nonTerminal.followSet | first
                if "E" in first or len(rule) == 0:
                    nt = self.nonTerminal(occurrence[0])
                    visitNonTerminal(nt)
                    nonTerminal.followSet = nonTerminal.followSet | nt.followSet

        for nonTerminal in nonTerminals.values():
            if nonTerminal.identifier == "__top__":
                nonTerminal.followSet.add("$")
            visitNonTerminal(nonTerminal)
            if "E" in nonTerminal.followSet:
                nonTerminal.followSet.remove("E")
