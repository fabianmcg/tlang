#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Grammar.parseElements import (
    EmptyString,
    Instruction,
    NonTerminal,
    Production,
    ProductionAttributes,
    ProductionKind,
    Rule,
    Terminal,
)
from Utility.util import getShortRepr
from copy import deepcopy


class Grammar:
    def __init__(self, lexer=None, tokens=None):
        self.tokens = {tok.parseRepr(): tok for tok in lexer.tokens.values()} if lexer else tokens
        self.terminals = {}
        self.nonTerminals = {}
        self.productions = {}
        self.productionsAsRules = {}
        self.specialProductions = {}

    def __str__(self) -> str:
        return "{}".format("\n".join(list(map(str, self.productions.values()))))

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

    def analysis(self):
        self.derivesEmpty()
        self.computeFirstSet()
        self.computeFollow()
        self.computeLL1()

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
        if k == len(symbolList):
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

    def computeLL1(self):
        productions = self.productions
        for production in productions.values():
            followSet = production.nonTerminal.followSet
            firstSets = [rule.firstSet for rule in production]
            disjoint = [True] * len(production)
            isLL1 = [True] * len(production)
            for i, rule in enumerate(production):
                for o, s in enumerate(firstSets[i + 1 :]):
                    if len(s.intersection(rule.firstSet)) != 0:
                        disjoint[i] = False
                        disjoint[o] = False
                    if "E" in s and len(followSet.intersection(rule.firstSet)) != 0:
                        isLL1[i] = False
                        isLL1[o] = False
                    if "E" in rule.firstSet and len(followSet.intersection(s)) != 0:
                        isLL1[i] = False
                        isLL1[o] = False
            for i, rule in enumerate(production):
                rule.isPredictable = disjoint[i]
                rule.isLL1 = isLL1[i] and disjoint[i]
            production.isPredictable = all(disjoint)
            production.isLL1 = all(isLL1 + disjoint)


def grammarForAnalysis(G: Grammar) -> Grammar:
    grammar = Grammar(tokens=G.tokens)
    specialProductions = grammar.specialProductions
    grammar.productions = {
        k: deepcopy(production)
        for k, production in G.productions.items()
        if production.attributes.kind == ProductionKind.Regular
    }
    productions = grammar.productions
    for k, production in G.productions.items():
        attributes = production.attributes
        if attributes.kind != ProductionKind.Regular:
            if k not in specialProductions:
                specialProductions[k] = []
            if attributes.kind == ProductionKind.ZeroOrMore:
                symbol = deepcopy(production[0][0])
                rules = [Rule([symbol, NonTerminal(k)], Instruction()), Rule([], Instruction())]
                productions[k] = Production(k, ProductionAttributes(attributes.returnType, attributes.isDynamic), rules)
                specialProductions[k].append(k)
            elif attributes.kind == ProductionKind.OneOrMore:
                identifier = "OOM_{}".format(k)
                symbol = deepcopy(production[0][0])
                rules = [Rule([symbol, NonTerminal(identifier)], Instruction())]
                productions[k] = Production(k, ProductionAttributes(attributes.returnType, attributes.isDynamic), rules)
                rules = [Rule([NonTerminal(k)], Instruction()), Rule([], Instruction())]
                productions[identifier] = Production(identifier, ProductionAttributes(), rules)
                specialProductions[k].append(k)
                specialProductions[k].append(identifier)
            elif attributes.kind == ProductionKind.Optional:
                symbol = deepcopy(production[0][0])
                rules = [Rule([symbol], Instruction()), Rule([], Instruction())]
                productions[k] = Production(k, ProductionAttributes(attributes.returnType, attributes.isDynamic), rules)
                specialProductions[k].append(k)
    return grammar
