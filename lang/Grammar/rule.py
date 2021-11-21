#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.util import getShort
from copy import deepcopy


class Metadata:
    def __add__(self, other):
        return Metadata()

    def __str__(self) -> str:
        return ""

    __repr__ = __str__


class RuleNode:
    def __init__(self, metadata=None):
        self.metadata = metadata or Metadata()

    def clone(self):
        return deepcopy(self)

    def getNode(self):
        return None

    def hasMetadata(self):
        return self.metadata != None

    def __invert__(self):
        return Optional(self.clone())

    def __add__(self, other):
        return And(self.clone(), other.clone())

    def __or__(self, other):
        return Or(self.clone(), other.clone())

    def longRepr(self) -> str:
        return (
            "{"
            + str(self.__class__.__name__)
            + ": "
            + (getShort(self.metadata) + " " if self.hasMetadata() else "")
            + getShort(self.getNode())
            + "}"
        )

    def shortRepr(self) -> str:
        return self.longRepr()

    def __str__(self) -> str:
        return self.shortRepr()

    __repr__ = __str__


class EmptyProduction(RuleNode):
    def __init__(self, metadata=None):
        super().__init__(metadata)

    def getNode(self):
        return "\u03B5"

    def shortRepr(self):
        return "\u03B5"


class Terminal(RuleNode):
    def __init__(self, identifier, metadata=None):
        super().__init__(metadata)
        self.identifier = identifier

    def getNode(self):
        return self.identifier

    def shortRepr(self):
        return "T." + getShort(self.identifier)


class NonTerminal(RuleNode):
    def __init__(self, identifier, metadata=None):
        super().__init__(metadata)
        self.identifier = identifier

    def getNode(self):
        return self.identifier

    def shortRepr(self):
        return "N." + getShort(self.identifier)


class ZeroOrMore(RuleNode):
    def __init__(self, node, metadata=None):
        super().__init__(metadata)
        self.node = node

    def getNode(self):
        return self.node

    def shortRepr(self):
        return getShort(self.node) + "*"


class OneOrMore(RuleNode):
    def __init__(self, node, metadata=None):
        super().__init__(metadata)
        self.node = node

    def getNode(self):
        return self.node

    def shortRepr(self):
        return getShort(self.node) + "+"


class Optional(RuleNode):
    def __init__(self, node, metadata=None):
        super().__init__(metadata)
        self.node = node

    def getNode(self):
        return self.node

    def shortRepr(self):
        return getShort(self.node) + "?"


def makeFlatNodeList(kind, x, y):
    if isinstance(x, kind) and isinstance(y, kind):
        return x.nodes + y.nodes
    if isinstance(x, kind) and not isinstance(y, kind):
        return x.nodes + [y]
    elif not isinstance(x, kind) and isinstance(y, kind):
        return [x] + y.nodes
    else:
        return [x, y]


class And(RuleNode):
    def __init__(self, x, y, metadata=None):
        super().__init__(metadata)
        self.nodes = makeFlatNodeList(And, x, y)

    def getNode(self):
        return self.nodes

    def shortRepr(self):
        return "(" + " & ".join([r.shortRepr() for r in self.nodes]) + ")"


class Or(RuleNode):
    def __init__(self, x, y, metadata=None):
        super().__init__(metadata)
        self.nodes = makeFlatNodeList(Or, x, y)

    def getNode(self):
        return self.nodes

    def shortRepr(self):
        return "(" + " | ".join([r.shortRepr() for r in self.nodes]) + ")"


from Utility.dotDict import DotDict

ruleDict = DotDict(
    {
        "E": EmptyProduction,
        "T": Terminal,
        "N": NonTerminal,
        "O": Optional,
        "ZM": ZeroOrMore,
        "OM": OneOrMore,
        "AD": And,
        "OR": Or,
    }
)


from Utility.type import NodeType, UniquePtr


class Rule:
    def __init__(self, identifier, isNode, returnType=None) -> None:
        self.identifier = identifier
        self.rules = []
        self.returnType = returnType if not isNode else UniquePtr(NodeType(identifier))

    def __enter__(self):
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        pass

    def __ilshift__(self, data):
        if isinstance(data, list):
            self.rules.extend(data)
        else:
            self.rules.append(data)
        return self

    def __str__(self) -> str:
        if len(self.rules) == 0:
            return "{} ::= \u03B5".format(self.identifier)
        else:
            return "{} ::=\n       {}".format(self.identifier, "\n    |  ".join(list(map(str, self.rules))))

    __repr__ = __str__

    def shortRepr(self):
        return str([k.shortRepr() for k in self.rules])
