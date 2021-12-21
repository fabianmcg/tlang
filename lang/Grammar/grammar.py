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


class ProductionData:
    def __init__(self) -> None:
        self.derivesEmpty = False

    def __str__(self) -> str:
        return str(self.derivesEmpty)

    def __repr__(self) -> str:
        return str(self)


class RuleData:
    def __init__(self, pid: str, rid: int) -> None:
        self.pid = pid
        self.rid = rid
        self.count = 0
        self.derivesEmpty = False


class NodeData:
    def __init__(self) -> None:
        self.production = None
        self.occurrences = set([])
        self.derivesEmpty = False
        self.first = set([])
        self.follow = set([])

    def __str__(self) -> str:
        return str(self.first)

    def __repr__(self) -> str:
        return str(self)


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

    def consolidate(self):
        terminals = self.terminals
        nonTerminals = self.nonTerminals
        for production in self.productions.values():
            if not production.data:
                production.data = ProductionData()
            if production.identifier not in nonTerminals:
                nonTerminals[production.identifier] = production.asNonTerminal().clone()
                nonTerminals[production.identifier].data = NodeData()
            for ruleId, rule in enumerate(production):
                if not rule.data:
                    rule.data = RuleData(production.identifier, ruleId)
                for node in rule:
                    symbol = None
                    if isinstance(node, Terminal):
                        if node.identifier not in terminals:
                            symbol = node.clone()
                            terminals[node.identifier] = symbol
                            symbol.data = NodeData()
                            symbol.data.first.add(symbol.identifier)
                        else:
                            symbol = terminals[node.identifier]
                    elif isinstance(node, NonTerminal):
                        if node.identifier not in nonTerminals:
                            symbol = node.clone()
                            nonTerminals[node.identifier] = symbol
                            symbol.data = NodeData()
                        else:
                            symbol = nonTerminals[node.identifier]
                    else:
                        raise (Exception("Invalid node"))
                    symbol.data.occurrences.add((production.identifier, ruleId))
        self.productionsAsRules = [rule for production in self.productions.values() for rule in production]

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
