#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""


class Instruction:
    def __init__(self, instruction) -> None:
        self.instruction = instruction

    def __str__(self) -> str:
        return str(self.instruction)

    __repr__ = __str__

    shortRepr = __str__


class VariableDecl(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VD::" + self.instruction.shortRepr()


class VariableRef(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VR::" + self.instruction


class ReturnStmt(Instruction):
    def shortRepr(self):
        return "R::" + self.instruction


from Utility.dotDict import DotDict

instructionDict = DotDict(
    {
        "I": Instruction,
        "VD": VariableDecl,
        "VR": VariableRef,
        "R": ReturnStmt,
    }
)
