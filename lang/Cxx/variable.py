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