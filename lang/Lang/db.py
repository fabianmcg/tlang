#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Lang.rule import InstructionRule, Match
from Lang.action import *
from Lang.instruction import instructionDict as ID
from Conf.toks import LToks
from Lang.type import NodeType

helperRules = DotDict()
astNodes = DotDict()

nodes = DotDictWrapper(astNodes, lambda x: Match(NodeAction(x)))
nodesTypes = DotDictWrapper(astNodes, lambda x: NodeType(x))
tokens = DotDictWrapper(LToks, lambda x: Match(TokenAction(x)))

instructionDict = DotDict(
    {
        "I": lambda x: InstructionRule(InstructionAction(ID.I(x))),
        "VD": lambda x: InstructionRule(InstructionAction(ID.VD(x))),
        "VR": lambda x: InstructionAction(ID.VR(x)),
        "RET": lambda x: InstructionRule(InstructionAction(ID.R(x))),
    }
)


def addNode(kind, identifier, dct={}):
    astNodes[identifier] = kind(identifier, **dct)
