#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from enum import Enum
from Utility.util import formatStr, getShortRepr, indentTxt
from copy import deepcopy


class Instruction:
    def __init__(self, instruction: str = "", lDelimiter=":{", rDelimiter="}:") -> None:
        format_str = False
        if isinstance(instruction, str):
            instruction = instruction.strip()
            if len(instruction) and format_str:
                instruction = indentTxt(formatStr(instruction), 4)
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
        return "{}\n{}\n{}".format(self.lDelimiter, self.instruction, self.rDelimiter) if len(self.instruction) else ""

    def cxx(self):
        return self.instruction


class AbstractNode:
    def __init__(self, identifier) -> None:
        self.identifier = identifier
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

    def parseStr(self):
        return "{}".format(self.identifier)

    def shortRepr(self):
        return "{}".format(self.identifier)


class Terminal(AbstractNode):
    def __init__(self, identifier) -> None:
        super().__init__(identifier)
        self.firstSet.add(identifier)

    def clone(self):
        return Terminal(self.identifier)


class NonTerminal(AbstractNode):
    def __init__(self, identifier) -> None:
        super().__init__(identifier)
        self.followSet = set([])
        self.derivesEmpty = False

    def clone(self):
        return NonTerminal(self.identifier)

    def info(self) -> str:
        return "{}\t{}".format(self.firstSet, self.followSet)


class EmptyString(NonTerminal):
    def __init__(self) -> None:
        super().__init__("E")


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

    def __deepcopy__(self, memo):
        cpy = Rule(deepcopy(self.rule), deepcopy(self.instruction))
        cpy.productionId = self.productionId
        cpy.firstSet = deepcopy(self.firstSet)
        cpy.countEmpty = self.countEmpty
        cpy.derivesEmpty = self.derivesEmpty
        cpy.isPredictable = self.isPredictable
        cpy.isLL1 = self.isLL1
        return cpy

    def isEmpty(self):
        return len(self.rule) == 0

    def hasInstruction(self):
        return self.instruction.notEmpty()

    def parseStr(self):
        instruction = "{}".format(self.instruction) if self.hasInstruction() else ""
        if len(instruction):
            instruction = indentTxt("\n" + instruction, 9)
        rule = " ".join(map(str, self.rule)) if len(self.rule) else "E"
        return "{}{}".format(rule, instruction)

    def shortRepr(self):
        return "{}".format(" ".join(map(getShortRepr, self.rule)))


class ProductionKind(Enum):
    Regular = 0
    ZeroOrMore = 1
    OneOrMore = 2
    Optional = 3


class ParsingMode(Enum):
    Deduced = 0
    LL1 = 1
    Predictive = 2
    OperatorParsing = 3


class ProductionAttributes:
    def __init__(
        self,
        returnType: str = "",
        isDynamic: bool = False,
        kind: ProductionKind = ProductionKind.Regular,
        mode: ParsingMode = ParsingMode.Deduced,
    ) -> None:
        returnType = returnType.strip()
        if len(returnType):
            returnType = formatStr(returnType)
        self.returnType = returnType
        self.isDynamic = isDynamic
        self.kind = kind
        self.mode = mode

    def __str__(self) -> str:
        return self.parseStr()

    def __repr__(self) -> str:
        return str(self)

    def hasReturnType(self):
        return len(self.returnType) > 0

    def parseStr(self):
        text = " @<{}>@".format(self.returnType) if len(self.returnType) else ""
        text += "" if self.isDynamic else " static"
        if self.kind != ProductionKind.Regular:
            text += " {}".format(self.kind.name)
        if self.mode != ParsingMode.Deduced:
            text += " {}".format(self.mode.name)
        return text

    def shortRepr(self):
        return ""


class Production:
    def __init__(self, identifier, attributes: ProductionAttributes, rules: list) -> None:
        self.identifier = identifier
        self.attributes = attributes
        self.rules = rules
        self.nonTerminal = NonTerminal(self.identifier)
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

    def __deepcopy__(self, memo):
        cpy = Production(self.identifier, deepcopy(self.attributes), [deepcopy(rule) for rule in self.rules])
        cpy.nonTerminal = deepcopy(self.nonTerminal)
        cpy.isPredictable = self.isPredictable
        cpy.isLL1 = self.isLL1
        return cpy

    def returnType(self):
        return self.attributes.returnType if self.attributes.hasReturnType() else self.identifier

    def isDynamic(self):
        return self.attributes.isDynamic

    def isTop(self):
        return "__top__" == self.identifier

    def isSimple(self):
        return (
            ProductionKind.Regular == self.attributes.kind
            and len(self.rules) == 1
            and len(self.rules[0]) == 1
            and isinstance(self.rules[0][0], NonTerminal)
        )

    def mode(self):
        return self.attributes.mode

    def hasEmpty(self):
        return any([r.isEmpty() for r in self.rules])

    def parseStr(self):
        n = len(self.rules)
        if n > 1:
            rules = "\n|".join([indentTxt((" " if i == 0 else "") + str(x), 8) for i, x in enumerate(self.rules)])
            rules = indentTxt(rules + "\n;", 7)
            rules = "\n" + rules
        else:
            rules = "\n" + indentTxt("{}\n;".format(indentTxt(" " + str(self.rules[0]), 8)), 7)
        return "{:}{:}:{}\n".format(self.identifier, self.attributes, rules)

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
