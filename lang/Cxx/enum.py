#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Cxx.util import CxxList
from Utility.util import indentTxt


class EnumElement:
    def __init__(self, identifier, value=None) -> None:
        self.identifier = identifier
        self.value = value

    def __str__(self) -> str:
        return self.identifier + (" = " + self.value if self.value else "")

    def __repr__(self) -> str:
        return str(self)


class Enum(CxxList):
    def __init__(self, identifier, isClass, values) -> None:
        self.identifier = identifier
        self.isClass = isClass
        self.value = values

    def parseStr(self, indentation=0):
        if self.empty():
            return ""
        tmp = indentTxt(",\n".join(map(str, self)), 2)
        return indentTxt(" [\n{}\n];\n".format(tmp), indentation)
