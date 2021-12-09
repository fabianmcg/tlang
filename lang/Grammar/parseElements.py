#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.util import formatStr, indentTxt


class Instruction:
    def __init__(self, instruction: str, lDelimiter="{", rDelimiter="}") -> None:
        if isinstance(instruction, str):
            instruction = instruction.strip()
            # if len(instruction):
            #     instruction = formatStr(instruction)
        self.instruction = instruction
        self.lDelimiter = lDelimiter
        self.rDelimiter = rDelimiter

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def empty(self):
        return len(self.instruction) == 0

    def notEmpty(self):
        return not self.empty()

    def parseStr(self):
        return "{} {} {}".format(self.lDelimiter, self.instruction, self.rDelimiter) if len(self.instruction) else ""


class NodeInstruction(Instruction):
    def __init__(self, instruction) -> None:
        super().__init__(instruction, "@{", "}@")


class RuleInstruction(Instruction):
    def __init__(self, instruction) -> None:
        super().__init__(instruction, ":{", "}:")


class AbstractNode:
    def __init__(self, identifier, instruction: Instruction) -> None:
        self.identifier = identifier
        self.instruction = instruction

    def __str__(self) -> str:
        instruction = "{}".format(self.instruction) if self.hasInstruction() else ""
        if len(instruction):
            instruction = " " + instruction
        return "{}{}".format(self.identifier, instruction)

    def __repr__(self) -> str:
        return str(self)

    def hasInstruction(self):
        return self.instruction.notEmpty()


class Terminal(AbstractNode):
    def __init__(self, identifier, instruction: Instruction) -> None:
        super().__init__(identifier, instruction=instruction)


class NonTerminal(AbstractNode):
    def __init__(self, identifier, instruction: Instruction) -> None:
        super().__init__(identifier, instruction=instruction)


class EmptyTerminal(NonTerminal):
    def __init__(self, instruction: Instruction) -> None:
        super().__init__("E", instruction)


class Rule:
    def __init__(self, rule, instruction: Instruction) -> None:
        self.rule = rule
        self.instruction = instruction

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def hasInstruction(self):
        return self.instruction.notEmpty()

    def parseStr(self):
        instruction = "{}".format(self.instruction) if self.hasInstruction() else ""
        if len(instruction):
            instruction = " " + instruction
        return "{}{}".format(" ".join(map(str, self.rule)), instruction)


class ProductionAttributes:
    def __init__(self, returnType: str, isNode: bool, instruction: Instruction) -> None:
        returnType = returnType.strip()
        if len(returnType):
            returnType = formatStr(returnType)
        self.returnType = returnType
        self.isNode = isNode
        self.instruction = instruction

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def hasInstruction(self):
        return self.instruction.notEmpty()

    def parseStr(self):
        text = " @< {} >@".format(self.returnType) if len(self.returnType) else ""
        text += " node" if self.isNode else ""
        text += " {}".format(self.instruction) if self.instruction.notEmpty() else ""
        return text


class Production:
    def __init__(self, identifier, attributes, rules) -> None:
        self.identifier = identifier
        self.attributes = attributes
        self.rules = rules

    def __str__(self) -> str:
        n = len(self.rules)
        if n > 1:
            rules = "\n|".join([indentTxt((" " if i == 0 else "") + str(x), 8) for i, x in enumerate(self.rules)])
            rules = indentTxt(rules + "\n;", 8)
            rules = "\n" + rules
        else:
            rules = " {};".format(self.rules[0])
        return "{:}{:}:={}\n".format(self.identifier, self.attributes, rules)

    def __repr__(self) -> str:
        return str(self)
