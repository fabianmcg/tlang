#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Lang.rule import Match


class ParseAction:
    def __init__(self, data=None):
        self.data = data

    def __add__(self, rhs):
        return Match(AppendAction(self, rhs))

    def __lshift__(self, rhs):
        return Match(AssignAction(self, rhs))

    def __str__(self) -> str:
        return str(self.data)

    __repr__ = __str__


class VariableAction(ParseAction):
    def __init__(self, node):
        super().__init__(node)


class TokenAction(ParseAction):
    def __init__(self, token):
        super().__init__(token)


class NodeAction(ParseAction):
    def __init__(self, node):
        super().__init__(node)


class HelperAction(ParseAction):
    def __init__(self, helper):
        super().__init__(helper)


class BinaryAction(ParseAction):
    def __init__(self, lhs, rhs):
        super().__init__()
        self.lhs = lhs
        self.rhs = rhs.rule if isinstance(rhs, Match) else rhs

    def __str__(self) -> str:
        return str(self.lhs) + ":" + str(self.rhs)


class AssignAction(BinaryAction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)


class AppendAction(BinaryAction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)
