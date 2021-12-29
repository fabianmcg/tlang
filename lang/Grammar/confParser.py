#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pyparsing as pp
from Utility.parsing import keywordList, parseOptional, suppressLiterals, suppressChars, parseElement, punctuation as P
from Grammar.parseElements import *
from Grammar.grammar import Grammar
from Lexer.lexer import Lexer


class Parser:
    def __init__(self, tokens: dict) -> None:
        self.tokens = tokens

    @staticmethod
    def parseInstruction(instructionClass):
        def parse(x):
            return instructionClass(x[0])

        return parse

    @staticmethod
    def createInstruction(begin, end, instructionClass):
        def makeLiteral(literal):
            return suppressChars(literal) if len(literal) == 1 else suppressLiterals(literal)

        return parseElement(makeLiteral(begin) + ... + makeLiteral(end), Parser.parseInstruction(instructionClass))

    def createRuleNode(self):
        node = P.identifier | pp.QuotedString('"')

        def action(x):
            if str(x[0]) in self.tokens:
                return Terminal(self.tokens[x[0]])
            elif str(x[0]) == "E":
                return EmptyString()
            return NonTerminal(*x)

        return parseElement(node, action)

    def createRule(self):
        ruleNode = self.createRuleNode()
        instruction = parseOptional(Parser.createInstruction(":{", "}:", Instruction), default=Instruction(""))

        def action(x):
            return Rule([symbol for symbol in x[0] if not isinstance(symbol, EmptyString)], x[1])

        return parseElement(pp.Group(pp.OneOrMore(ruleNode)) + instruction, action)

    def createProductionAttributes(self):
        lat, rat = suppressLiterals("@<", ">@")
        returnType = parseOptional(lat + ... + rat, lambda x: ("returnType", x[0]), "")
        dynamic = parseOptional(pp.Keyword("static"), lambda x: ("isDynamic", True if x[0] == True else False), True)
        kind = pp.Keyword("ZeroOrMore") | pp.Keyword("OneOrMore") | pp.Keyword("Optional")

        def getKind(x):
            x = x[0]
            kind = ProductionKind.Regular
            if x == "ZeroOrMore":
                kind = ProductionKind.ZeroOrMore
            elif x == "OneOrMore":
                kind = ProductionKind.OneOrMore
            elif x == "Optional":
                kind = ProductionKind.Optional
            return ("kind", kind)

        kind = parseOptional(kind, getKind, None)

        def action(x):
            return ProductionAttributes(**{k: v for k, v in x})

        return parseElement(returnType + dynamic + kind, action)

    def createProduction(self):
        pipe = suppressChars("|")
        equal = suppressLiterals(":=")
        attributes = parseOptional(
            self.createProductionAttributes(), default=ProductionAttributes("", False, Instruction(""))
        )
        rule = self.createRule()
        semicolon = suppressChars(";")
        return parseElement(
            P.identifier + attributes + equal + pp.Group(rule + pp.ZeroOrMore(pipe + rule) + semicolon),
            lambda x: Production(*x),
        )

    def createProductionList(self):
        return pp.OneOrMore(self.createProduction())

    def parse(self, filename):
        pp._enable_all_warnings()
        return self.createProductionList().ignore(pp.cStyleComment).parseFile(filename, parseAll=True)


def makeParser(filename, lexer: Lexer):
    grammar = Grammar(lexer)
    grammar.setProductions(Parser(grammar.tokens).parse(filename))
    return grammar
