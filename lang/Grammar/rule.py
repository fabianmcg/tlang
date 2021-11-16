#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.util import makeList, makeListVariadic, getShort


class ParseException(Exception):
    pass


class RuleNode:
    def __init__(self, data=None, instruction=None):
        self.data = data
        self.atomic = True
        self.instruction = instruction

    @staticmethod
    def atomicList(x):
        return makeList(x if x.atomic else x.data)

    @staticmethod
    def getRepr(x):
        return getShort(x) if not isinstance(x, RuleNode) else x.shortRepr()

    def __invert__(self):
        return Optional(self)

    def __add__(self, other):
        if isinstance(self, And) or isinstance(other, And):
            return And(self.atomicList(self) + self.atomicList(other))
        else:
            return And(self, other)

    def __or__(self, other):
        if isinstance(self, Or) or isinstance(other, Or):
            return Or(self.atomicList(self) + self.atomicList(other))
        else:
            return Or(self, other)

    def __pow__(self, rhs):
        rhs.instruction = AppendInstruction(self, rhs.data)
        return rhs

    def __lshift__(self, rhs):
        rhs.instruction = AssignInstruction(self, rhs.data)
        return rhs

    def shortRepr(self) -> str:
        return (
            "{"
            + str(self.__class__.__name__)
            + ": "
            + (str(self.instruction) + " " if self.hasInstruction() else "")
            + str(self.data)
            + "}"
        )

    def hasInstruction(self):
        return self.instruction != None

    def __str__(self) -> str:
        return self.shortRepr()

    __repr__ = __str__


class Match(RuleNode):
    def __init__(self, rule, instruction=None):
        super().__init__(rule, instruction)

    def shortRepr(self):
        return RuleNode.getRepr(self.data)


class ZeroOrMore(RuleNode):
    def __init__(self, rule):
        super().__init__(rule)

    def shortRepr(self):
        return RuleNode.getRepr(self.data) + "*"


class OneOrMore(RuleNode):
    def __init__(self, rule):
        super().__init__(rule)

    def shortRepr(self):
        return RuleNode.getRepr(self.data) + "+"


class Optional(RuleNode):
    def __init__(self, rule):
        super().__init__(rule)

    def shortRepr(self):
        return RuleNode.getRepr(self.data) + "?"


class And(RuleNode):
    def __init__(self, *rules):
        super().__init__(makeListVariadic(*rules))
        self.atomic = False

    def shortRepr(self):
        return "(" + " & ".join([r.shortRepr() for r in self.data]) + ")"


class Or(RuleNode):
    def __init__(self, *rules):
        super().__init__(makeListVariadic(*rules))
        self.atomic = False

    def shortRepr(self):
        return "(" + " | ".join([r.shortRepr() for r in self.data]) + ")"


class Instruction(RuleNode):
    def __init__(self, instruction=None):
        super().__init__(instruction=instruction)

    def __str__(self) -> str:
        return self.shortRepr()

    def shortRepr(self):
        return str(None) if self.instruction == None else "I:" + RuleNode.getRepr(self.instruction)


class VariableDecl(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VD::" + self.instruction.shortRepr()


class VariableRef(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VR::" + self.instruction


class ReturnStmt(Instruction):
    def shortRepr(self):
        return "R::" + self.instruction


class BinaryInstruction(Instruction):
    def __init__(self, lhs, rhs):
        super().__init__()
        self.lhs = lhs
        self.rhs = rhs

    def __str__(self) -> str:
        return "<{} : {}>".format(str(self.lhs), str(self.rhs))


class AssignInstruction(BinaryInstruction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)

    def __str__(self) -> str:
        return "<{} = {}>".format(str(self.lhs), str(self.rhs))


class AppendInstruction(BinaryInstruction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)

    def __str__(self) -> str:
        return "<{} += {}>".format(str(self.lhs), str(self.rhs))


from Utility.dotDict import DotDict

ruleDict = DotDict(
    {
        "I": Instruction,
        "M": Match,
        "O": Optional,
        "ZM": ZeroOrMore,
        "OM": OneOrMore,
        "AD": And,
        "OR": Or,
    }
)


class Symbol:
    def __init__(self, identifier):
        self.identifier = identifier

    def __str__(self) -> str:
        return getShort(self.identifier)

    __repr__ = __str__

    def shortRepr(self):
        return str(self)


class Terminal(Symbol):
    def __str__(self) -> str:
        return "T." + getShort(self.identifier)


class NonTerminal(Symbol):
    def __str__(self) -> str:
        return "N." + getShort(self.identifier)


from Utility.type import NodeType


class Rule:
    def __init__(self, identifier, isNode, returnType=None) -> None:
        self.identifier = identifier
        self.rules = []
        self.isNode = isNode
        self.returnType = returnType if not isNode else NodeType(identifier)

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
