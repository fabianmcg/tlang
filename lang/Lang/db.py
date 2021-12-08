#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Utility.struct import Class
from Lexer.db import LexerDB
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
                "Size": SizeType(),
                "Float": FloatType(),
                "Void": VoidType(),
                "String": StringType(),
            }
        )
        self.definedTypes = DotDict()
        self.grammar = GrammarDB(tokens)
        self.nodesByClass = {}

    def __str__(self) -> str:
        return "Nodes:\n{}\n{}".format("\n".join(list(map(str, self.nodes.values()))), str(self.grammar))

    __repr__ = __str__

    def addNode(self, identifier, addRule=True, **kwargs):
        self.nodes[identifier] = Node(identifier, **kwargs)
        self.types[identifier] = NodeType(identifier)
        if addRule:
            self.grammar.addRule(identifier)
        return self.nodes[identifier]

    def genAddNode(self, classOf, nodeClass=None):
        nodeClass = "Node" or nodeClass

        def addNode(identifier, addRule=True, **kwargs):
            if classOf not in self.nodesByClass:
                self.nodesByClass[classOf] = {}
            node = self.addNode(identifier, addRule, classOf=nodeClass, **kwargs)
            self.nodesByClass[classOf][identifier] = node
            return node

        return addNode

    def addType(self, identifier, T=None):
        self.definedTypes[identifier] = T or Class(identifier)
        self.types[identifier] = StructType(identifier)
        return self.types[identifier]

    def getNodes(self, function=lambda x: x):
        return DotDictWrapper(self.nodes, function)

    def getGrammar(self):
        return self.grammar.rules, self.grammar.nonTerminals, self.grammar.terminals
