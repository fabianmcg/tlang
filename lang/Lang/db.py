#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Utility.dotDict import DotDict, DotDictWrapper
from Lang.rule import Match
from Lang.action import *
from Conf.toks import LToks

astNodes = DotDict()

nodes = DotDictWrapper(astNodes, lambda x: Match(NodeAction(x)))
toks = DotDictWrapper(LToks, lambda x: Match(TokenAction(x)))


class MemberAccessor:
    def __init__(self, node):
        self.node = node

    def __getattr__(self, attr):
        return VariableAction(self.node.__getattr__(attr))

vars = DotDictWrapper(astNodes, lambda x: MemberAccessor(x))

def addNode(kind, identifier, dct={}):
    astNodes[identifier] = kind(identifier, **dct)
