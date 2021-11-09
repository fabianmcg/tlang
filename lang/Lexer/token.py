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
        return self.identifier + ": " + (str(self.rules[0]) if len(self.rules) == 1 else str(self.rules))

    __repr__ = __str__

    def shortRepr(self):
        return self.identifier


class Keyword(Token):
    def __init__(self, identifier, keyword=None):
        super().__init__(identifier, keyword if keyword != None else identifier.lower())


class Literal(Token):
    def __init__(self, identifier, keyword):
        super().__init__(identifier, keyword)


class Character(Token):
    def __init__(self, identifier, character):
        super().__init__(identifier, character)


class Rule(Token):
    def __init__(self, identifier, *rules):
        super().__init__(identifier, *rules)


class Definition(Token):
    def __init__(self, identifier, *rules):
        super().__init__(identifier, *rules)
