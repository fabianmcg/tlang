#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Lexer.db import LexerDB
from Lang.node import Node
from Lang.type import NodeType
from Lang.rule import Match, InstructionRule
from Lang.action import TokenAction, NodeAction, InstructionAction
from Lang.instruction import *


class LangDB:
    def __init__(self, tokens: LexerDB):
        self.tokens = tokens
        self.nodes = DotDict()
        self.extraRules = DotDict()

    def __str__(self) -> str:
        return "Nodes:\n{}".format("\n".join(list(map(str, self.nodes.values()))))

    __repr__ = __str__

    def addNode(self, identifier, dct={}):
        self.nodes[identifier] = Node(identifier, **dct)

    def getNodes(self, function=lambda x: x):
        return DotDictWrapper(self.nodes, function)

    def getNodesTypes(self):
        return self.getNodes(lambda x: NodeType(x))

    def getParseTokens(self):
        return self.tokens.getTokens(lambda x: Match(TokenAction(x)))

    def getParseNodes(self):
        return DotDictWrapper(self.nodes, lambda x: Match(NodeAction(x)))

    def getNodesIdentifiers(self):
        return DotDictWrapper(self.nodes, lambda x: x.identifier)


instructionDict = DotDict(
    {
        "I": lambda x: InstructionRule(InstructionAction(Instruction(x))),
        "VD": lambda x: InstructionRule(InstructionAction(VariableDecl(x))),
        "VR": lambda x: InstructionAction(VariableRef(x)),
        "RET": lambda x: InstructionRule(InstructionAction(ReturnStmt(x))),
    }
)
