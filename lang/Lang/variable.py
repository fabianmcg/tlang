#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

import Lang.type as Type


class Variable:
    def __init__(self, T, identifier):
        self.T = T
        self.identifier = identifier

    def __str__(self) -> str:
        return str(self.T) + ": " + self.identifier

    __repr__ = __str__

    def shortRepr(self):
        return self.identifier


class AutoVariable(Variable):
    def __init__(self, identifier):
        super().__init__(Type.AutoType(), identifier)


class NodeVariable(Variable):
    def __init__(self, T, identifier):
        super().__init__(Type.NodeType(T), identifier)


class VectorVariable(Variable):
    def __init__(self, T, identifier):
        super().__init__(Type.Vector(T), identifier)


class UniquePtrVariable(Variable):
    def __init__(self, T, identifier):
        super().__init__(Type.UniquePtr(T), identifier)


class UniquePtrVectorVariable(Variable):
    def __init__(self, T, identifier):
        super().__init__(Type.Vector(Type.UniquePtr(T)), identifier)


from Utility.dotDict import DotDict

variableDict = DotDict(
    {
        "V": Variable,
        "A": AutoVariable,
        "N": NodeVariable,
        "VV": VectorVariable,
        "UP": UniquePtrVariable,
        "UV": UniquePtrVectorVariable,
    }
)
