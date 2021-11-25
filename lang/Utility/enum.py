#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum


class Enumeration:
    def __init__(self, identifier, enum, isClass=False) -> None:
        self.identifier = identifier
        self.enum = enum
        self.isClass = isClass

    def __str__(self) -> str:
        return str(repr(self.enum))

    __repr__ = __str__

    @staticmethod
    def create(identifier, tags, isClass=False):
        return Enumeration(identifier, Enum(identifier, tags, start=0), isClass)
