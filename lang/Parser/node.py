#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum
from collections.abc import Iterable

NodeKind = Enum("NodeKind", ["base", "parsable"])


class Node:
    @staticmethod
    def makeList(x):
        return x if isinstance(x, list) else (list(x) if isinstance(x, Iterable) else [x])

    def __init__(self, parents="ASTNode", members={}, rules=[], methods={}, kind=NodeKind.base):
        self.parents = Node.makeList(parents)
        self.members = members
        self.rules = Node.makeList(rules)
        self.methods = methods
        self.kind = kind


class BaseNode(Node):
    def __init__(self, parents="ASTNode"):
        super().__init__(parents=parents)

class ParsableNode(Node):
    def __init__(self, parents="ASTNode", members={}, rules=[], methods={}):
        super().__init__(parents=parents, members=members, rules=rules, methods=methods, kind=NodeKind.parsable)
