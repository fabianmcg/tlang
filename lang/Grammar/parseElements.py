#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from numpy.core.fromnumeric import prod
from Utility.util import formatStr, getShortRepr, indentTxt


class Instruction:
    def __init__(self, instruction: str, lDelimiter="{", rDelimiter="}") -> None:
        if isinstance(instruction, str):
            instruction = instruction.strip()
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

    def cxx(self):
        return self.instruction


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
        self.firstSet = set([])
        self.occurrencesSet = []

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def __hash__(self) -> int:
        return hash(self.identifier)

    def __eq__(self, other):
        return other.identifier == self.identifier and (
            (isinstance(self, Terminal) and isinstance(other, Terminal))
            or (isinstance(self, NonTerminal) and isinstance(other, NonTerminal))
        )

    def hasInstruction(self):
        return self.instruction.notEmpty()

    def parseStr(self):
        instruction = "{}".format(self.instruction) if self.hasInstruction() else ""
        if len(instruction):
            instruction = " " + instruction
        return "{}{}".format(self.identifier, instruction)

    def shortRepr(self):
        return "{}".format(self.identifier)


class Terminal(AbstractNode):
    def __init__(self, identifier, instruction: Instruction) -> None:
        super().__init__(identifier, instruction=instruction)
        self.firstSet.add(identifier)

    def clone(self):
        return Terminal(self.identifier, Instruction(""))


class NonTerminal(AbstractNode):
    def __init__(self, identifier, instruction: Instruction) -> None:
        super().__init__(identifier, instruction=instruction)
        self.followSet = set([])
        self.derivesEmpty = False

    def clone(self):
        return NonTerminal(self.identifier, Instruction(""))

    def info(self) -> str:
        return "{}\t{}".format(self.firstSet, self.followSet)


class EmptyString(NonTerminal):
    def __init__(self, instruction=None) -> None:
        super().__init__("E", instruction or Instruction(""))


class Rule:
    def __init__(self, rule: list, instruction: Instruction) -> None:
        self.rule = rule
        self.instruction = instruction
        self.productionId = None
        self.countEmpty = 0
        self.firstSet = set([])
        self.derivesEmpty = False
        self.isPredictable = False
        self.isLL1 = False

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def __len__(self):
        return len(self.rule)

    def __iter__(self):
        return self.rule.__iter__()

    def __getitem__(self, item):
        return self.rule[item]

    def isEmpty(self):
        return len(self.rule) == 0

    def hasInstruction(self):
        return self.instruction.notEmpty()

    def parseStr(self):
        instruction = "{}".format(self.instruction) if self.hasInstruction() else ""
        if len(instruction):
            instruction = " " + instruction
        rule = " ".join(map(str, self.rule)) if len(self.rule) else "E"
        return "{}{}".format(rule, instruction)

    def shortRepr(self):
        return "{}".format(" ".join(map(getShortRepr, self.rule)))


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

    def hasReturnType(self):
        return len(self.returnType) > 0

    def parseStr(self):
        text = " @< {} >@".format(self.returnType) if len(self.returnType) else ""
        text += " node" if self.isNode else ""
        text += " {}".format(self.instruction) if self.instruction.notEmpty() else ""
        return text

    def shortRepr(self):
        return ""


class Production:
    def __init__(self, identifier, attributes: ProductionAttributes, rules: list) -> None:
        self.identifier = identifier
        self.attributes = attributes
        self.rules = rules
        self.nonTerminal = NonTerminal(self.identifier, Instruction(""))
        for rule in self.rules:
            rule.productionId = self.identifier
        self.isPredictable = False
        self.isLL1 = False

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def __eq__(self, other):
        return other.identifier == self.identifier

    def __len__(self):
        return len(self.rules)

    def __iter__(self):
        return self.rules.__iter__()

    def __getitem__(self, item):
        return self.rules[item]

    def __hash__(self) -> int:
        return hash(self.identifier)

    def isTop(self):
        return "__top__" == self.identifier

    def hasEmpty(self):
        return any([r.isEmpty() for r in self.rules])

    def parseStr(self):
        n = len(self.rules)
        if n > 1:
            rules = "\n|".join([indentTxt((" " if i == 0 else "") + str(x), 8) for i, x in enumerate(self.rules)])
            rules = indentTxt(rules + "\n;", 7)
            rules = "\n" + rules
        else:
            rules = " {};".format(self.rules[0])
        return "{:}{:}:={}\n".format(self.identifier, self.attributes, rules)

    def shortRepr(self):
        n = len(self.rules)
        if n > 1:
            rules = "\n|".join(
                [indentTxt((" " if i == 0 else "") + x.shortRepr(), 7) for i, x in enumerate(self.rules)]
            )
            rules = indentTxt(rules + "\n;", 8)
            rules = "\n" + rules
        else:
            rules = " {};".format(self.rules[0].shortRepr())
        return "{:}:={}\n".format(self.identifier, rules)
