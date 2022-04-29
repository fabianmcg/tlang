#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.type import Type


class Variable:
    def __init__(self, T, identifier: str):
        self.T = Type.check(T)
        self.identifier = identifier

    def __str__(self) -> str:
        return str(self.T) + " " + self.identifier

    def __repr__(self) -> str:
        return str(self)

    def varName(self):
        return "__" + self.identifier

    def cxx(self):
        return str(self.T) + " " + self.varName()

    def getCxx(self):
        return "{}& get{}() {{ return {};}}\n".format(
            self.T, self.identifier.capitalize(), self.varName()
        ) + "const {}& get{}() const {{ return {};}}".format(self.T, self.identifier.capitalize(), self.varName())
