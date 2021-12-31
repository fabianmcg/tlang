#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class Token:
    def __init__(self, identifier, *rules):
        self.identifier = identifier
        self.rules = list(rules)

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def __hash__(self) -> int:
        return hash(self.identifier)

    def __eq__(self, other):
        return other.identifier == self.identifier

    def cxx(self):
        return "tok_k::{}".format(self.identifier)

    def parseRepr(self):
        return self.rules[0]

    def parseStr(self):
        return self.parseRepr()


class Keyword(Token):
    def __init__(self, identifier, keyword=None):
        super().__init__(identifier, keyword if keyword != None else identifier.lower())


class Punctuation(Token):
    def __init__(self, identifier, character):
        super().__init__(identifier, character)

    def parseStr(self):
        return '"{}"'.format(self.rules[0])


class Operator(Punctuation):
    def __init__(self, identifier, character, name=None):
        super().__init__(identifier, character)
        self.name = name if name else identifier

    def parseStr(self):
        return '"{}"'.format(self.rules[0])


class Rule(Token):
    def __init__(self, identifier, *rules):
        super().__init__(identifier, *rules)

    def parseRepr(self):
        return self.identifier


class Definition(Token):
    def __init__(self, identifier, *rules):
        super().__init__(identifier, *rules)
