#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import pyparsing as pp



def suppressChars(chars):
    if len(chars) > 1:
        return map(lambda x: pp.Char(x).suppress(), chars)
    return pp.Char(chars).suppress()


def suppressLiterals(*literals):
    if len(literals) > 1:
        return map(lambda x: pp.Literal(x).suppress(), literals)
    return pp.Literal(literals[0]).suppress()


class Parser:
    identifier = pp.Word(pp.alphas + "_", pp.alphanums + "_")
    @staticmethod
    def createProductionList():
        return pp.OneOrMore(Parser.createProduction()).ignore(pp.cStyleComment)

    @staticmethod
    def parse(filename):
        pp._enable_all_warnings()
        result = Parser.createProductionList().parseFile(filename, parseAll=True)
        print("\n".join(map(str, list(result))))


def parse(filename):
    parser = Parser.parse(filename)