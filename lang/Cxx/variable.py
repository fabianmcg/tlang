#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import Type


class Variable:
    def __init__(self, T, identifier):
        self.T = Type.check(T)
        self.identifier = identifier

    def __str__(self) -> str:
        return str(self.T) + " " + self.identifier

    def __repr__(self) -> str:
        return str(self)


class StaticNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("static " + self.identifier)

    def cxx(self):
        return self.__str__()

    def typename(self) -> str:
        return str(self)


class DynamicNode(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)


class DynamicList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("list " + self.identifier)


class StaticList(Type):
    def __init__(self, identifier: str):
        super().__init__(identifier)

    def parseStr(self, indentation=0):
        return str("static list " + self.identifier)


class ChildNode(Variable):
    pass
