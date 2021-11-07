#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class Type:
    def __init__(self, T):
        self.T = T

    def __str__(self) -> str:
        return str(self.T)

    __repr__ = __str__

    def isTemplate(self):
        return False

    def underlying(self):
        return self.T


class TemplateType(Type):
    def __init__(self, TT, T):
        super().__init__(Type(T))
        self.TT = TT

    def __str__(self) -> str:
        return self.TT.format(str(self.T))

    def isTemplate(self):
        return True

    def underlying(self):
        return self.T


class Vector(TemplateType):
    def __init__(self, T):
        super().__init__("std::vector<{}>", Type(T))


class UniquePtr(TemplateType):
    def __init__(self, T):
        super().__init__("std::unique_ptr<{}>", Type(T))


class AutoType(Type):
    def __init__(self):
        super().__init__("auto")


class BoolType(Type):
    def __init__(self):
        super().__init__("bool")


class IntType(Type):
    def __init__(self):
        super().__init__("int")


class FloatType(Type):
    def __init__(self):
        super().__init__("float")


class VoidType(Type):
    def __init__(self):
        super().__init__("void")


from Utility.dotDict import DotDict

typeDict = DotDict(
    {
        "T": Type,
        "A": AutoType,
        "B": BoolType,
        "I": IntType,
        "F": FloatType,
        "V": VoidType,
        "TT": TemplateType,
        "VT": Vector,
        "UP": UniquePtr,
    }
)
