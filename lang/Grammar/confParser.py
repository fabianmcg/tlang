#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from glob import glob
import pyparsing as pp
from Utility.parsing import (
    parseOptional,
    suppressKeywords,
    suppressLiterals,
    suppressChars,
    parseElement,
    punctuation as P,
)
from Grammar.parseElements import *
from Grammar.grammar import Grammar
from Lexer.lexer import Lexer
from pathlib import Path


class ImportDirective:
    def __init__(self, filename: str) -> None:
        self.filename = filename

    def __str__(self) -> str:
        return str(self.filename)

    def __repr__(self) -> str:
        return str(self)


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
        mode = pp.Keyword("LL1") | pp.Keyword("Predictive") | pp.Keyword("OperatorParsing")

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

        def getMode(x):
            x = x[0]
            kind = ParsingMode.Deduced
            if x == "LL1":
                kind = ParsingMode.LL1
            elif x == "Predictive":
                kind = ParsingMode.Predictive
            elif x == "OperatorParsing":
                kind = ParsingMode.OperatorParsing
            return ("mode", kind)

        kind = parseOptional(kind, getKind, None)
        mode = parseOptional(mode, getMode, None)

        def action(x):
            return ProductionAttributes(**{k: v for k, v in x})

        return parseElement(returnType + dynamic + kind + mode, action)

    def createProduction(self):
        pipe = suppressChars("|")
        equal = suppressLiterals(":")
        attributes = parseOptional(
            self.createProductionAttributes(), default=ProductionAttributes("", False, Instruction(""))
        )
        rule = self.createRule()
        semicolon = suppressChars(";")
        return parseElement(
            P.identifier + attributes + equal + pp.Group(rule + pp.ZeroOrMore(pipe + rule) + semicolon),
            lambda x: Production(*x),
        )

    def createImport(self):
        qs = pp.QuotedString('"')
        return parseElement(suppressKeywords("import") + qs, lambda x: ImportDirective(x[0]))

    def createProductionList(self):
        return pp.OneOrMore(self.createProduction() | self.createImport())

    def parse(self, filename):
        pp._enable_all_warnings()
        return self.createProductionList().ignore(pp.cStyleComment).parseFile(filename, parseAll=True)


def processFile(filename: str, grammar: Grammar):
    print('Parsing "{}" grammar'.format(filename))
    file = Parser(grammar.tokens).parse(filename)
    productions = [x for x in file if type(x) != ImportDirective]
    imports = [x.filename for x in file if type(x) == ImportDirective]
    return productions, imports


def makeParser(filename, lexer: Lexer):
    grammar = Grammar(lexer)
    productions = []
    imports = []
    resolvedImports = set([filename])
    p, i = processFile(filename, grammar)
    productions.extend(p)
    imports.extend(i)
    while len(imports) > 0:
        fn = imports.pop(0)
        if fn in resolvedImports:
            continue
        resolvedImports.add(fn)
        if "*" in fn:
            print(glob(fn), fn)
            imports.extend(glob(fn))
        else:
            path = Path(fn)
            if not path.exists():
                print('Cannot import "{}", path doesn\'t exist!'.format(fn))
                continue
            p, i = processFile(fn, grammar)
            productions.extend(p)
            imports.extend(i)
    grammar.setProductions(productions)
    return grammar
