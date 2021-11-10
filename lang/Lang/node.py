#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from collections.abc import Iterable
from Lang.rule import RuleList


class Node:
    @staticmethod
    def makeList(x):
        return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) and not isinstance(x, str) else [x])

    @staticmethod
    def makeMembers(x):
        return {v.identifier: v for v in x}

    def __init__(self, identifier, parents="ASTNode", members=[], methods={}):
        self.identifier = identifier
        self.parents = Node.makeList(parents)
        self.members = Node.makeMembers(members)
        self.rules = RuleList()
        self.methods = methods

    def __ilshift__(self, rule):
        self.rules <<= rule
        return self

    def __getattr__(self, k):
        if k in self.members:
            return self.members[k]
        elif k in self.methods:
            return self.methods[k]
        return None

    def __str__(self) -> str:
        return "{}: {{\n  {}\n  {}\n  {}\n}}".format(
            self.identifier,
            str(self.parents),
            str(list(self.members.values())),
            self.rules.shortRepr(),
        )

    __repr__ = __str__

    def shortRepr(self):
        return self.identifier
