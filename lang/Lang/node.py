#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""
from Utility.struct import Class


class ChildrenType:
    def __init__(self, Type, Identifier, Visit=True) -> None:
        self.T = Type
        self.identifier = Identifier
        self.visit = Visit

    def __str__(self) -> str:
        return "{} {}".format(self.T, self.identifier)

    __repr__ = __str__


class Static(ChildrenType):
    def __init__(self, Type, Identifier, Visit=True) -> None:
        super().__init__(Type, Identifier, Visit)


class Dynamic(ChildrenType):
    def __init__(self, Type, Identifier, Visit=True) -> None:
        super().__init__(Type, Identifier, Visit)


class Reference(ChildrenType):
    def __init__(self, Type, Identifier, Visit=False) -> None:
        super().__init__(Type, Identifier, Visit)


class ManyStatic(ChildrenType):
    def __init__(self, Type, Identifier, Visit=True) -> None:
        super().__init__(Type, Identifier, Visit)


class ManyDynamic(ChildrenType):
    def __init__(self, Type, Identifier, Visit=True) -> None:
        super().__init__(Type, Identifier, Visit)


class ChildrenList:
    def __init__(self, *data):
        self.data = list(data)

    def __str__(self) -> str:
        return "[" + ", ".join(list(map(str, self.data))) + "]"

    __repr__ = __str__

    def __ilshift__(self, x):
        self.data.extend(x.data)
        return self

    def __iadd__(self, x):
        self.data.append(x)
        return self

    def empty(self):
        return len(self.data) == 0


class Node(Class):
    def __init__(self, typename, base=False, classOf=None, loc=False):
        super().__init__(typename)
        self.children = ChildrenList()
        self.classOf = classOf
        self.hasLocation = loc
        self.base = base
        self.defined = True if self.base else self.defined

    def __ilshift__(self, x):
        if isinstance(x, ChildrenList):
            self.children <<= x
        else:
            Class.__ilshift__(self, x)
        return self
