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
        instruction = parseOptional(Parser.createInstruction("@{", "}@", NodeInstruction), default=NodeInstruction(""))

        def action(x):
            if str(x[0]) in self.tokens:
                return Terminal(self.tokens[x[0]], x[1])
            elif str(x[0]) == "E":
                return EmptyString(x[1])
            return NonTerminal(*x)

        return parseElement(node + instruction, action)

    def createRule(self):
        ruleNode = self.createRuleNode()
        instruction = parseOptional(Parser.createInstruction(":{", "}:", RuleInstruction), default=RuleInstruction(""))

        def action(x):
            return Rule([symbol for symbol in x[0] if not isinstance(symbol, EmptyString)], x[1])

        return parseElement(pp.Group(pp.OneOrMore(ruleNode)) + instruction, action)

    def createProductionAttributes(self):
        lat, rat = suppressLiterals("@<", ">@")
        instruction = Parser.createInstruction(":{", "}:", RuleInstruction)
        returnType = parseOptional(lat + ... + rat, lambda x: ("returnType", x[0]), "")
        inline = parseOptional(pp.Keyword("node"), lambda x: ("isNode", True), False)
        instruction = parseOptional(instruction, lambda x: ("instruction", x[0]), RuleInstruction(""))

        def action(x):
            return ProductionAttributes(**{k: v for k, v in x})

        return parseElement(returnType + inline + instruction, action)

    def createProduction(self):
        pipe = suppressChars("|")
        equal = suppressLiterals(":=")
        attributes = parseOptional(
            self.createProductionAttributes(), default=ProductionAttributes("", False, RuleInstruction(""))
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
