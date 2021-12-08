#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict
from Utility.util import jinjaTemplate, pathJoin, printToFile
from Lexer.token import *


class Lexer:
    def __init__(self):
        self.definitions = DotDict()
        self.tokens = DotDict()

    def __getattr__(self, attr):
        return self.tokens.__getattr__(attr)

    def __str__(self) -> str:
        return "Definitions:\n\t{}\nTokens:\n\t{}".format(
            "\n\t".join(list(map(str, self.definitions.values()))), "\n\t".join(list(map(str, self.tokens.values())))
        )

    __repr__ = __str__

    def addToken(self, kind, identifier, *rules):
        self.tokens[identifier] = kind(identifier, *rules)

    def addDefinition(self, identifier, *rules):
        self.definitions[identifier] = Definition(identifier, *rules)

    def generateLexer(self, outputDir: str, inputDir):
        tokens = self.tokens
        enum = {}
        switch = {}
        definitions = {k: v.rules[0] for k, v in self.definitions.items()}
        rules = {}
        for k, v in tokens.items():
            if isinstance(v, Keyword):
                enum[k] = v.rules[0]
                switch[k] = enum[k]
                rules['"{}"'.format(v.rules[0])] = k
            elif isinstance(v, Rule):
                enum[k] = " | ".join(v.rules)
                switch[k] = k
                for r in v.rules:
                    rules["{}".format(r)] = k
            elif isinstance(v, Punctuation):
                if len(v.rules[0]) == 1:
                    enum["{} = '{}'".format(k, v.rules[0])] = v.rules[0]
                else:
                    enum[k] = v.rules[0]
                switch[k] = v.rules[0]
                rules['"{}"'.format(v.rules[0])] = k

        jinjaTemplate(
            pathJoin(outputDir, "tokens.hh"), pathJoin(inputDir, "tokens.hh.j2"), {"enum": enum, "switch": switch}
        )
        jinjaTemplate(
            pathJoin(outputDir, "lex.yy"), pathJoin(inputDir, "lex.yy.j2"), {"definitions": definitions, "rules": rules}
        )
