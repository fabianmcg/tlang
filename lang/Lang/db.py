#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Utility.type import *
from Utility.struct import Class
from Lexer.db import LexerDB
from Lang.node import Node
from Grammar.db import GrammarDB


class LangDB:
    def __init__(self, tokens: LexerDB):
        self.tokens = tokens
        self.nodes = DotDict()
        self.types = DotDict(
            {
                "Auto": AutoType(),
                "Bool": BoolType(),
                "Int": IntType(),
                "Float": FloatType(),
                "Void": VoidType(),
                "String": StringType(),
            }
        )
        self.nodesByClass = {}
        self.grammar = GrammarDB(tokens)

    def __str__(self) -> str:
        return "Nodes:\n{}\n{}".format("\n".join(list(map(str, self.nodes.values()))), str(self.grammar))

    __repr__ = __str__

    def addNode(self, identifier, addRule=True, **kwargs):
        self.nodes[identifier] = Node(identifier, **kwargs)
        self.types[identifier] = NodeType(identifier)
        if addRule:
            self.grammar.addRule(identifier)
        return self.nodes[identifier]

    def genAddNode(self, fileName, classOf=None):
        classOf = fileName if classOf == None else classOf
        key = (fileName, classOf)

        def addNode(identifier, addRule=True, **kwargs):
            if key not in self.nodesByClass:
                self.nodesByClass[key] = {}
            self.nodes[identifier] = Node(identifier, classOf=classOf, **kwargs)
            self.types[identifier] = NodeType(identifier)
            self.nodesByClass[key][identifier] = self.nodes[identifier]
            if addRule:
                self.grammar.addRule(identifier)
            return self.nodes[identifier]

        return addNode

    def addType(self, identifier, T=None):
        self.types[identifier] = T or Class(identifier)
        return self.types[identifier]

    def getNodes(self, function=lambda x: x):
        return DotDictWrapper(self.nodes, function)

    def getGrammar(self):
        return self.grammar.rules, self.grammar.nonTerminals, self.grammar.terminals
