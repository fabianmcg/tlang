#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.util import getShort
from Grammar.rule import Metadata, RuleNode


class Instruction(Metadata):
    def __init__(self, instruction=None):
        super().__init__()
        self.instruction = instruction

    def __str__(self) -> str:
        return self.shortRepr()

    def __pow__(self, rhs: RuleNode):
        x = rhs.clone()
        x.metadata = AppendInstruction(self, rhs)
        return x

    def __lshift__(self, rhs: RuleNode):
        x = rhs.clone()
        x.metadata = AssignInstruction(self, rhs)
        return x

    def shortRepr(self):
        return str(None) if self.instruction == None else "I:" + getShort(self.instruction)


class VariableDecl(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VD::" + getShort(self.instruction)


class VariableRef(Instruction):
    def __init__(self, variable) -> None:
        super().__init__(variable)

    def shortRepr(self):
        return "VR::" + getShort(self.instruction)


class ReturnStmt(Instruction):
    def shortRepr(self):
        return "R::" + getShort(self.instruction)


class BinaryInstruction(Instruction):
    def __init__(self, lhs, rhs):
        super().__init__()
        self.lhs = lhs
        self.rhs = rhs

    def shortRepr(self) -> str:
        return "<{} : {}>".format(str(self.lhs), str(self.rhs))


class AssignInstruction(BinaryInstruction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)

    def shortRepr(self) -> str:
        return "<{} = {}>".format(str(self.lhs), str(self.rhs))


class AppendInstruction(BinaryInstruction):
    def __init__(self, lhs, rhs):
        super().__init__(lhs, rhs)

    def shortRepr(self) -> str:
        return "<{} += {}>".format(str(self.lhs), str(self.rhs))
