#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

class Variable:
    def __init__(self, identifier, Type):
        self.identifier = identifier
        self.Type = Type

    def accessor(self):
        return "."

    def __str__(self) -> str:
        return str(self.Type) + ": " + self.identifier 

    __repr__ = __str__

class UniquePtrV(Variable):
    def __init__(self, identifier, Type):
        super().__init__(identifier, Type)