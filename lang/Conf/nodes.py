#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Oct Sun 31 11:09:00 2021

@author: fabian
"""

from Parser.rule import TokenRule
from Conf.toks import LToks


astNodes = {}


class RToks:
    def __getattr__(self, attr):
        return TokenRule(LToks.__getattr__(attr))


RTok = RToks()


def addNode(kind, identifier, *rules):
    astNodes[identifier] = kind(identifier, *rules)
