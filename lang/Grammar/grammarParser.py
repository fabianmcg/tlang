#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Grammar.grammar import Grammar, grammarForAnalysis
from Grammar.parseElements import EmptyString, NonTerminal, Production, ProductionKind, Rule, Terminal
from Utility.util import jinjaTemplate, pathJoin, format


class GrammarParser:
    def __init__(self, grammar: Grammar) -> None:
        self.grammar = grammar
        self.analysisGrammar = grammarForAnalysis(grammar)
        self.grammar.consolidate()
        self.analysisGrammar.consolidate()
        self.analysisGrammar.analysis()

    def predictSet(self, rule: Rule, production: Production):
        predict = rule.firstSet.copy()
        if "E" in predict:
            predict.remove("E")
            predict = predict | production.nonTerminal.followSet
        return predict

    def cxxRule(self, rule: Rule, production: Production, aProduction: Production, ruleId: int):
        if rule.isEmpty() and production.attributes.kind != ProductionKind.Regular:
            return "return _r;"
        if rule.isEmpty():
            return "return return_t::empty();"
        src = "return_t _r;"
        for i, symbol in enumerate(production[ruleId]):
            if isinstance(symbol, Terminal):
                src += "auto _{} = match({}, \"{}\");".format(i, symbol.identifier.cxx(), production.identifier)
            elif isinstance(symbol, NonTerminal):
                src += "auto _{} = Parse{}();".format(i, symbol.identifier)
                src += "check(_{}, \"{}\", \"{}\");".format(i, symbol.identifier, production.identifier)
        src += production[ruleId].instruction.cxx()
        return src + "return _r;"

    def cxxProductionLL1(self, production: Production, aProduction: Production):
        cases = ""
        def cxxCase(terminal):
            if isinstance(terminal, str):
                return "case tok::EOF:"
            return "case {}:".format(terminal.cxx())
        for i, rule in enumerate(aProduction):
            predict = self.predictSet(rule, aProduction)
            tmp = "\n".join(map(cxxCase, predict))
            tmp += "{"
            tmp += self.cxxRule(rule, production, aProduction, i)
            tmp += "}"
            cases += tmp

        return "switch (peek().kind) {{{} default: syntaxError(\"{}\");}}".format(cases, production.identifier)

    def cxxProductionNLL1(self, production: Production, aProduction: Production):
        cases = ""
        def cxxCase(terminal):
            if isinstance(terminal, str):
                return "case tok::EOF:"
            return "case {}:".format(terminal.cxx())
        for i, rule in enumerate(aProduction):
            predict = self.predictSet(rule, aProduction)
            tmp = "\n".join(map(cxxCase, predict))
            tmp += "{"
            tmp += self.cxxRule(rule, production, aProduction, i)
            tmp += "}"
            cases += tmp

        return "switch (peek().kind) {{{} default: syntaxError(__LINE__, __func__);}}".format(cases)

    def cxxProduction(self, production: Production, aProduction: Production):
        tmp = "Dynamic" if production.isDynamic() else "Static"
        returnType = "parse_result<{}, return_kind::{}>".format(production.returnType(), tmp)
        header = "using return_t = parse_result<{}, return_kind::{}>;".format(production.returnType(), tmp)
        body = "{}"
        epilogue = "return return_t::fail();"
        if production.attributes.kind != ProductionKind.Regular:
            body = "{{return_t _r; while(true) {{{}}}}}"
        if aProduction.isLL1:
            body = body.format(self.cxxProductionLL1(production, aProduction))
        else:
            body = body.format(self.cxxProductionNLL1(production, aProduction))
        return returnType, "{}{}{}".format(header, body, epilogue)

    def cxxTop(self, production: Production):
        if production.isTop():
            body = "return Parse{}();".format(str(production[0][0]))
            return "auto ParseTop() {{{}}}".format(body)
        else:
            identifier = production.identifier
            aProduction = self.analysisGrammar.productions[production.identifier]
            returnType, body = self.cxxProduction(production, aProduction)
            return "{} Parse{}() {{{}}}".format(returnType, identifier, body)

    def cxx(self):
        grammar = self.grammar
        productions = grammar.productions
        p = [self.cxxTop(production) for production in productions.values()]
        # for production in productions.values():
        #     print(production.identifier, production.nonTerminal.firstSet, production.nonTerminal.followSet)
        return "\n".join(p)

    def generateParser(self, outputPath, templatePath):
        jinjaTemplate(
            pathJoin(outputPath, "ParserHandler.cc"),
            pathJoin(templatePath, "Parser.cc.j2"),
            {"PARSE_METHODS": self.cxx()},
        )
        format(pathJoin(outputPath, "ParserHandler.cc"))
