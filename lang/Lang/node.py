#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from collections.abc import Iterable
from Utility.struct import Class
from Lang.rule import RuleList, Rule


class ParsableClass(Class):
    @staticmethod
    def makeList(x):
        return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) and not isinstance(x, str) else [x])

    def __init__(self, typename):
        super().__init__(typename)
        self.rules = RuleList()

    def __ilshift__(self, x):
        Class.__ilshift__(self, x)
        if isinstance(x, Rule):
            self.rules <<= x
        return self

    def __iadd__(self, x):
        Class.__iadd__(self, x)
        if isinstance(x, Rule):
            self.rules <<= x
        return self

    def __str__(self) -> str:
        return "{}: {{\n Parents:\n  {}\n Members:\n  {}\n Rules:\n  {}\n}}".format(
            self.T,
            str(self.parents),
            str(self.members),
            self.rules.shortRepr(),
        )

    __repr__ = __str__

    def shortRepr(self):
        return self.typename()


class Node(ParsableClass):
    pass
