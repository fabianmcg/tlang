#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum
from collections.abc import Iterable

RuleKind = Enum("RuleKind", ["Token", "Node", "ZeroOrMore", "OneOrMore", "And", "Or", "Maybe"])


class Rule:
    def __init__(self, kind, rule=None):
        self.kind = kind
        self.rule = rule
        self.atomic = True

    @staticmethod
    def makeList(x):
        return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) else [x])

    @staticmethod
    def makeVList(*x):
        return Rule.makeList(x[0]) if len(x) == 1 else list(x)

    @staticmethod
    def atomicList(x):
        return Rule.makeList(x if x.atomic else x.rule)

    def __add__(self, other):
        if self.kind == RuleKind.And or other.kind == RuleKind.And:
            return And(self.atomicList(self) + self.atomicList(other))
        else:
            return And(self, other)

    def __or__(self, other):
        if self.kind == RuleKind.Or or other.kind == RuleKind.Or:
            return Or(self.atomicList(self) + self.atomicList(other))
        else:
            return Or(self, other)

    def __invert__(self):
        return Maybe(self)

    def __str__(self) -> str:
        return "{" + str(self.kind) + ": " + str(self.rule) + "}"

    __repr__ = __str__


class TokenRule(Rule):
    def __init__(self, token):
        super().__init__(RuleKind.Token, token)


class NodeRule(Rule):
    def __init__(self, node):
        super().__init__(RuleKind.Node, node)


class And(Rule):
    def __init__(self, *rules):
        super().__init__(RuleKind.And, Rule.makeVList(*rules))
        self.atomic = False


class Or(Rule):
    def __init__(self, *rules):
        super().__init__(RuleKind.Or, Rule.makeVList(*rules))
        self.atomic = False


class ZeroOrMore(Rule):
    def __init__(self, rule):
        super().__init__(RuleKind.ZeroOrMore, rule)


class OneOrMore(Rule):
    def __init__(self, rule):
        super().__init__(RuleKind.OneOrMore, rule)


class Maybe(Rule):
    def __init__(self, rule):
        super().__init__(RuleKind.Maybe, rule)


from Conf.toks import LTok


class RToks:
    def __getattr__(self, attr):
        return TokenRule(LTok.__getattr__(attr))


RTok = RToks()
