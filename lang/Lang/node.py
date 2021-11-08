#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from collections.abc import Iterable


class Node:
    @staticmethod
    def makeList(x):
        return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) and not isinstance(x, str) else [x])

    @staticmethod
    def makeMembers(x):
        return {v.identifier: v for v in x}

    def __init__(self, identifier, parents="ASTNode", members=[], rules=[], methods={}):
        self.identifier = identifier
        self.parents = Node.makeList(parents)
        self.members = Node.makeMembers(members)
        self.rules = Node.makeList(rules)
        self.methods = methods

    def __ilshift__(self, rule):
        if isinstance(rule, list):
            self.rules.extend(rule)
        else:
            self.rules.append(rule)
        return self

    def __getattr__(self, k):
        if k in self.members:
            return self.members[k]
        elif k in self.methods:
            return self.methods[k]
        return None

    def __str__(self) -> str:
        return "{}: {{\n\t{}\n\t{}\n\t{}\n}}".format(
            self.identifier, str(self.parents), str(list(self.members.values())), str(self.rules)
        )

    __repr__ = __str__
